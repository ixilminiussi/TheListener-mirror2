import csv
import waapi

WORD_SPEED = 2.4

class Tag:
    def __init__(self):
        self.total               = ""
        self.day                 = 0
        self.alt                 = ""
        self.conversation_number = 0
        self.dialogue_number     = 0
        self.line_number         = 0
        self.time_override       = 0.0

class SubtitleInfo:
    def __init__(self, word_count, tag):
        self.word_count = word_count
        self.tag = tag

class SubtitleGroup:
    def __init__(self, day, alt, conversation_number, dialogue_number):
        self.day = day
        self.alt = alt
        self.conversation_number = conversation_number
        self.dialogue_number = dialogue_number
        self.subtitles = []
        self.wwise_id = None

def parse_line(parts):
    """Parse CSV parts into tag, speaker, subtitle"""
    tag = Tag()

    tag.total = parts[0]
    
    tag_parts = parts[0].split('.')
    idx = 0
    
    # Parse day
    tag.day = int(tag_parts[idx])
    idx += 1
    
    # Parse conversation number
    tag.conversation_number = int(tag_parts[idx])
    idx += 1
    
    # Parse dialogue number
    tag.dialogue_number = int(tag_parts[idx])
    idx += 1
    
    # Check for alt (A, B, C, D, E)
    if idx < len(tag_parts) and len(tag_parts[idx]) == 1 and tag_parts[idx] in ['A', 'B', 'C', 'D', 'E']:
        tag.alt = tag_parts[idx]
        idx += 1
    
    # Check for line number (starts with S)
    if idx < len(tag_parts) and tag_parts[idx].startswith('S'):
        tag.line_number = int(tag_parts[idx][1:])
        idx += 1
    
    # Parse speaker name (second CSV column)
    speaker = parts[1] if len(parts) > 1 else ""
    
    # Parse subtitle (third CSV column onwards)
    subtitle = ' '.join(parts[2:]) if len(parts) > 2 else ""

    if len(parts) > 5:
        try:
            tag.time_override = float(parts[5])
        except (TypeError, ValueError):
            tag.time_override = 0
    
    return tag, speaker, subtitle

def count_words(text):
    """Count words in a string"""
    if not text:
        return 0
    return len(text.split())

def get_tag_name(group):
    """Generate tag name from group"""
    tag_name = f"audio_{group.day}_{group.conversation_number}_{group.dialogue_number}"
    if group.alt:
        tag_name += f"_{group.alt}"
    return tag_name

def get_event_id(group, prefix="play"):
    """Generate event ID from group"""
    event_id = f"{prefix}_{group.day}_{group.conversation_number}_{group.dialogue_number}"
    if group.alt:
        event_id += f"_{group.alt}"
    return event_id

def calculate_total_duration(group):
    """Calculate total duration of all subtitles in a group"""
    total_words = sum(subtitle.word_count for subtitle in group.subtitles)
    return total_words / WORD_SPEED

# WAAPI functions
def get_silent_placeholder_id(client):
    args = {"waql": '$ from type sound where Name = "Silence_1min_PlaceHolder"'}
    options = {"return": ["id", "name", "path"]}
    
    result = client.call(
        "ak.wwise.core.object.get",
        args,
        options
    )
    
    obj = result.get("return", [])[0]
    return obj.get("id")

def create_new_silent_placeholder(client):
    parent_path = "\\Actor-Mixer Hierarchy\\Automatic"
    
    result = client.call(
        "ak.wwise.core.object.copy", 
        {
            "object": get_silent_placeholder_id(client),
            "parent": parent_path,
            "onNameConflict": "replace"
        }, 
        {"return": ["id"]}
    )
    
    return result.get("id");

def create_new_silent(client, name):
    object_id = create_new_silent_placeholder(client)
    rename_args = {
        "object": object_id,
        "value": name,
    }
    
    try:
        client.call("ak.wwise.core.object.setName", rename_args)
    except Exception as e:
        print(f"Warning: Could not rename to '{name}': {e}")
    
    return object_id

def create_event(client, event_name, target_id, action_type="Play"):
    """Create a play or stop event for a target sound object"""
    parent_path = "\\Events\\Default Work Unit\\Automatic"
    
    # Create the event
    event_args = {
        "parent": parent_path,
        "type": "Event",
        "name": event_name,
        "onNameConflict": "replace"
    }
    
    event_result = client.call(
        "ak.wwise.core.object.create",
        event_args,
        {"return": ["id"]}
    )
    
    event_id = event_result.get("id")
    
    # Create the action (Play or Stop)
    action_args = {
        "parent": event_id,
        "type": "Action",
        "name": "",
        "onNameConflict": "replace"
    }
    
    action_result = client.call(
        "ak.wwise.core.object.create",
        action_args,
        {"return": ["id"]}
    )
    
    action_id = action_result.get("id")
    
    # Set the action type (Play or Stop)
    client.call("ak.wwise.core.object.setProperty", {
        "object": action_id,
        "property": "ActionType",
        "value": 1 if action_type == "Play" else 2  # 1=Play, 2=Stop
    })
    
    # Set the target reference
    client.call("ak.wwise.core.object.setReference", {
        "object": action_id,
        "reference": "Target",
        "value": target_id
    })
    
    return event_id

def add_markers(client, group):
    time = 0
    markers = []

    args = {"waql": '$ from type AudioSource where parent = "{' + group.wwise_id + '}"'}
    options = {"return": ["id", "name", "path"]}
    
    result = client.call(
        "ak.wwise.core.object.get",
        args,
        options
    )
    result = client.call("ak.wwise.core.object.get", {
        "from": {
            "id": [group.wwise_id]
        },
        "transform": [
            {"select": ["children"]}
        ]
    }, {
        "return": ["id", "name", "type"]
    })

    audio_source_id = result.get("return", [])[0].get("id")

    for subtitle_info in group.subtitles:
        markers.append({
                        "type": "Marker",
                        "name": "",
                        "@Time": time,
                        "@Label": subtitle_info.tag.total
                    })

        if subtitle_info.tag.time_override != 0:
            time += subtitle_info.tag.time_override;
        else:
            time += subtitle_info.word_count / WORD_SPEED;

        time = min(time, 59.9)

    markers.append({
        "type": "Marker",
        "name": "",
        "@Time": time,
        "@Label": "MOD_EARLYANSWER"
    });

    client.call("ak.wwise.core.object.set", {
        "objects": [
            {
                "object": audio_source_id,
                "@MarkerInputMode": 2,
                "@Markers" : markers,
                "listMode": "append"
            }
        ],
    })
    

def clear(client, path):
    result = client.call("ak.wwise.core.object.get", {
        "from": {
            "path": [path]
        },
        "transform": [
            {"select": ["children"]}
        ]
    }, {
        "return": ["id", "name", "type"]
    })

    for obj in result.get("return", []):
        client.call("ak.wwise.core.object.delete", {
            "object": obj.get("id")
        })

def main():
    subtitle_groups = []
    
    # Read CSV file
    with open('subtitles.csv', 'r', encoding='utf-8') as file:
        csv_reader = csv.reader(file)
        
        for parts in csv_reader:
            if not parts:  # Skip empty lines
                continue
            
            tag, speaker, subtitle = parse_line(parts)
            
            word_count = count_words(subtitle)
            subtitle_info = SubtitleInfo(word_count, tag)
            
            # Find existing group or create new one
            found = False
            for group in subtitle_groups:
                if (tag.day == group.day and
                    tag.conversation_number == group.conversation_number and
                    tag.alt == group.alt and
                    tag.dialogue_number == group.dialogue_number):
                    group.subtitles.append(subtitle_info)
                    found = True
                    break
            
            if not found:
                group = SubtitleGroup(
                    tag.day,
                    tag.alt,
                    tag.conversation_number,
                    tag.dialogue_number
                )
                group.subtitles.append(subtitle_info)
                subtitle_groups.append(group)
    
    print(f"Found {len(subtitle_groups)} subtitle groups\n")
    
    # Connect to WAAPI and create sound objects and events
    client = waapi.connect()
    if not client:
        print("ERROR: Could not connect to WAAPI")
        return
    
    try:
        clear(client, "\\Events\\Default Work Unit\\Automatic")
        clear(client, "\\Actor-Mixer Hierarchy\\Automatic")

        for group in subtitle_groups:
            tag_name = get_tag_name(group)
            
            print(f"Creating sound for: {tag_name}")
            print(f"  Subtitles: {len(group.subtitles)}")
            
            # Create the silent audio object in Wwise
            wwise_id = create_new_silent(client, tag_name)
            group.wwise_id = wwise_id
            print(f"  Created sound with ID: {wwise_id}")
            
            # Create play event
            play_event_name = get_event_id(group, "play")
            play_event_id = create_event(client, play_event_name, wwise_id, "Play")
            print(f"  Created play event: {play_event_name} (ID: {play_event_id})")
            
            # Create stop event
            stop_event_name = get_event_id(group, "stop")
            stop_event_id = create_event(client, stop_event_name, wwise_id, "Stop")
            print(f"  Created stop event: {stop_event_name} (ID: {stop_event_id})")

            add_markers(client, group)
            print(f"  Created markers")
            print()
            
    except Exception as e:
        print(f"WAAPI call failed: {e}")
        import traceback
        traceback.print_exc()
    
    finally:
        client.disconnect()

if __name__ == "__main__":
    main()