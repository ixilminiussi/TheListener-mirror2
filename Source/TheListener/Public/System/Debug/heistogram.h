// Credits to oldmoe/heistogram @ https://github.com/oldmoe/heistogram
#ifndef HEISTOGRAM_H
#define HEISTOGRAM_H

#include "CoreMinimal.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static float HEIST_GROWTH_FACTOR = 0.02;
static float HEIST_INV_LOG_GROWTH_FACTOR = 35.00278878; //116.2767475;
static uint16 HEIST_MAX_UNMAPPED_BUCKET = 57; // after this point multiple values can fall in the same bucket
static uint16 HEIST_BUCKET_MAPPING_DELTA = 147;
// 205 - 57 - 1; //shift the mappings to continue from the last artificial one

// Simplified Bucket structure - only stores count
struct FBucket
{
	uint64 count;
};

// Updated Heistogram structure
struct FHeistogram
{
	uint16 capacity; // Current capacity of buckets array
	uint16 min_bucket_id; // Smalles bucket id, for optimizing serialization
	uint64 total_count; // Total count of all values
	uint64 min; // Minimum value
	uint64 max; // Maximum value
	FBucket* buckets; // Array of buckets, index = bucket ID
};


/*************************/
/* VARINT HELPER METHODS */
/*************************/

// Returns number of bytes written
static inline size_t encode_varint(uint64 val, uint8* buf)
{
	if (val <= 240)
	{
		buf[0] = static_cast<uint8>(val);
		return 1;
	}
	if (val <= 2287)
	{
		val -= 240;
		buf[0] = static_cast<uint8>((val >> 8) + 241);
		buf[1] = static_cast<uint8>(val & 0xFF);
		return 2;
	}
	if (val <= 67823)
	{
		val -= 2288;
		buf[0] = 249;
		buf[1] = static_cast<uint8>(val >> 8);
		buf[2] = static_cast<uint8>(val & 0xFF);
		return 3;
	}

	// Determine number of bytes needed
	int bytes;
	if (val <= 0xFFFFFF)
	{
		bytes = 3;
	}
	else if (val <= 0xFFFFFFFF)
	{
		bytes = 4;
	}
	else if (val <= 0xFFFFFFFFFF)
	{
		bytes = 5;
	}
	else if (val <= 0xFFFFFFFFFFFF)
	{
		bytes = 6;
	}
	else if (val <= 0xFFFFFFFFFFFFFF)
	{
		bytes = 7;
	}
	else
	{
		bytes = 8;
	}

	buf[0] = 249 + bytes; // 250-255 based on size
	for (int i = bytes; i > 0; i--)
	{
		buf[i] = static_cast<uint8>(val & 0xFF);
		val >>= 8;
	}
	return bytes + 1;
}

// Returns number of bytes read
static inline size_t decode_varint(const uint8* buf, uint64* val)
{
	uint8 a0 = buf[0];

	if (a0 <= 240)
	{
		*val = a0;
		return 1;
	}
	if (a0 <= 248)
	{
		*val = 240 + 256 * (a0 - 241) + buf[1];
		return 2;
	}
	if (a0 == 249)
	{
		*val = 2288 + 256 * buf[1] + buf[2];
		return 3;
	}

	int bytes = a0 - 249; // Number of bytes following
	uint64 result = 0;
	for (int i = 1; i <= bytes; i++)
	{
		result = (result << 8) | buf[i];
	}
	*val = result;
	return bytes + 1;
}

/***********************/
/* MATH HELPER METHODS */
/***********************/

static inline double fast_pow_int(float x, int y)
{
	if (y == 0)
	{
		return 1.0f;
	}
	if (y < 0)
	{
		return 1.0f / fast_pow_int(x, -y); // Handle negative exponents
	}

	double result = 1.0f;
	while (y > 0)
	{
		if (y & 1)
		{
			// If y is odd
			result *= x;
		}
		x *= x; // Square x
		y >>= 1; // Divide y by 2 (right shift)
	}
	return result;
}

/*****************************/
/* HEISTOGRAM HELPER METHODS */
/*****************************/

static inline int16_t get_bucket_id(double value)
{
	if (value <= HEIST_MAX_UNMAPPED_BUCKET)
	{
		return value;
	}
	return static_cast<int16_t>(static_cast<float>(log2(value)) * HEIST_INV_LOG_GROWTH_FACTOR) -
		HEIST_BUCKET_MAPPING_DELTA;
}

static inline uint64 get_bucket_min(uint16 bid)
{
	if (bid <= HEIST_MAX_UNMAPPED_BUCKET)
	{
		return bid;
	}
	return static_cast<uint64>(ceil(fast_pow_int(1 + HEIST_GROWTH_FACTOR, bid + HEIST_BUCKET_MAPPING_DELTA)));
}

static inline uint64 get_bucket_max(uint32 min)
{
	if (min <= HEIST_MAX_UNMAPPED_BUCKET)
	{
		return min;
	}
	return static_cast<uint64>(min * HEIST_GROWTH_FACTOR) + min;
}

static inline size_t encode_bucket(uint32 count, uint8* buffer)
{
	uint8* ptr = buffer;
	ptr += encode_varint(count, ptr);
	return ptr - buffer;
}

static inline size_t encode_empty_buckets(uint32 count, uint8* buffer)
{
	for (uint32 i = 0; i < count; i++)
	{
		buffer[i] = 0;
	}
	return count;
}

static inline size_t decode_header(const uint8* buffer,
                                   uint16* bucket_count, uint64* total_count, uint64* min, uint64* max,
                                   uint16* min_bucket_id)
{
	const uint8* ptr = buffer;
	size_t bytes_read;
	uint64 temp;

	// Read bucket count
	bytes_read = decode_varint(ptr, &temp);
	if (bytes_read == 0)
	{
		return 0;
	}
	*bucket_count = static_cast<uint16>(temp);
	ptr += bytes_read;

	// Read total count
	bytes_read = decode_varint(ptr, &temp);
	if (bytes_read == 0)
	{
		return 0;
	}
	*total_count = temp;
	ptr += bytes_read;

	// Read min value
	bytes_read = decode_varint(ptr, &temp);
	if (bytes_read == 0)
	{
		return 0;
	}
	*min = temp;
	ptr += bytes_read;

	// Read max delta value
	bytes_read = decode_varint(ptr, &temp);
	if (bytes_read == 0)
	{
		return 0;
	}
	*max = temp + *min;
	ptr += bytes_read;

	// Read max bucket id
	bytes_read = decode_varint(ptr, &temp);
	if (bytes_read == 0)
	{
		return 0;
	}
	*min_bucket_id = static_cast<uint16>(temp);
	ptr += bytes_read;

	return ptr - buffer;
}

static inline size_t decode_bucket(const uint8* buffer, uint64* count)
{
	const uint8* ptr = buffer;
	size_t bytes_read;
	uint64 temp;

	bytes_read = decode_varint(ptr, &temp);
	if (bytes_read == 0)
	{
		return 0;
	}
	*count = temp;
	ptr += bytes_read;

	return ptr - buffer;
}

/**************************/
/* HEISTOGRAM API METHODS */
/**************************/

static FHeistogram* heistogram_create(void)
{
	FHeistogram* h = static_cast<FHeistogram*>(FMemory::Malloc(sizeof(FHeistogram)));
	if (!h)
	{
		return nullptr;
	}

	h->capacity = 16;
	h->total_count = 0;
	h->max = 0;
	h->min = 0;
	h->min_bucket_id = 0;

	// Initialize all buckets with zero count
	h->buckets = static_cast<FBucket*>(FMemory::Malloc(h->capacity * sizeof(FBucket)));
	if (!h->buckets)
	{
		FMemory::Free(h);
		return nullptr;
	}
	FMemory::Memzero(h->buckets, sizeof(FBucket) * h->capacity);

	return h;
}

static void heistogram_free(FHeistogram* h)
{
	if (!h)
	{
		return;
	}
	FMemory::Free(h->buckets);
	FMemory::Free(h);
}

static uint64 heistogram_count(const FHeistogram* h)
{
	return h ? h->total_count : 0;
}

static uint64 heistogram_max(const FHeistogram* h)
{
	return h ? h->max : 0;
}

static uint64 heistogram_min(const FHeistogram* h)
{
	return h ? h->min : 0;
}

static uint32 heistogram_memory_size(const FHeistogram* h)
{
	return h ? sizeof(FHeistogram) + (sizeof(FBucket) * h->capacity) : 0;
}

static void heistogram_add(FHeistogram* h, uint64 value)
{
	if (!h || value < 0)
	{
		return;
	}

	int16_t bid = get_bucket_id(value);
	if (h->total_count == 0)
	{
		h->min = value;
		h->max = value;
		h->min_bucket_id = bid;
	}
	else
	{
		if (h->min > value)
		{
			h->min = value;
		}
		if (h->max < value)
		{
			h->max = value;
		}
		if (bid < h->min_bucket_id)
		{
			h->min_bucket_id = bid;
		}
	}

	// Expand array if needed
	if (bid >= h->capacity)
	{
		size_t new_capacity = bid + 16; // + 16; // Add some extra space
		FBucket* new_buckets = static_cast<FBucket*>(FMemory::Realloc(h->buckets, new_capacity * sizeof(FBucket)));
		if (!new_buckets)
		{
			return;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + h->capacity, 0, (new_capacity - h->capacity) * sizeof(FBucket));

		h->buckets = new_buckets;
		h->capacity = new_capacity;
	}

	// Increment count in the appropriate bucket
	h->buckets[bid].count++;
	h->total_count++;
}

static void heistogram_remove(FHeistogram* h, uint64 value)
{
	if (!h || value < 0 || h->total_count == 0)
	{
		return;
	}

	int16_t bid = get_bucket_id(value);
	if (bid >= h->capacity || h->buckets[bid].count == 0)
	{
		return;
	}

	h->buckets[bid].count--;
	h->total_count--;

	// Check if histogram is now empty
	if (h->total_count == 0)
	{
		h->min = 0;
		h->max = 0;
		h->min_bucket_id = 0;
		return;
	}

	// Only update min/max values if the bucket count went to zero
	if (h->buckets[bid].count == 0)
	{
		// Check if this was the bucket containing the max value
		uint64 bucket_min = get_bucket_min(bid);
		uint64 bucket_max = get_bucket_max(bucket_min);

		if (h->max <= bucket_max && h->max >= bucket_min)
		{
			// Find new max by scanning buckets backward from the highest bucket
			h->max = 0;
			for (int16_t i = h->capacity - 1; i >= 0; i--)
			{
				if (h->buckets[i].count > 0)
				{
					// For the max, we use the upper bound of the bucket range
					uint64 b_min = get_bucket_min(i);
					uint64 b_max = get_bucket_max(b_min);
					h->max = b_max;
					break;
				}
			}
		}

		// Check if this was the bucket containing the min value
		if (h->min >= bucket_min && h->min <= bucket_max)
		{
			// Find new min by scanning buckets forward from the lowest bucket
			h->min = UINT64_MAX;
			for (int16_t i = 0; i < h->capacity; i++)
			{
				if (h->buckets[i].count > 0)
				{
					// For the min, we use the lower bound of the bucket range
					h->min = get_bucket_min(i);
					h->min_bucket_id = i;
					break;
				}
			}
		}

		// Update min_bucket_id if needed
		if (bid == h->min_bucket_id)
		{
			// Find new min_bucket_id
			for (int16_t i = h->min_bucket_id; i < h->capacity; i++)
			{
				if (h->buckets[i].count > 0)
				{
					h->min_bucket_id = i;
					break;
				}
			}
		}
	}
}

static FHeistogram* heistogram_merge(const FHeistogram* h1, const FHeistogram* h2)
{
	if (!h1 || !h2)
	{
		return nullptr;
	}

	FHeistogram* result = heistogram_create();
	if (!result)
	{
		return nullptr;
	}

	// Find the maximum capacity needed
	uint16 max_capacity = h1->capacity > h2->capacity ? h1->capacity : h2->capacity;

	// Resize result to accommodate all buckets
	if (max_capacity > result->capacity)
	{
		FBucket* new_buckets = static_cast<FBucket*>(FMemory::Realloc(result->buckets, max_capacity * sizeof(FBucket)));
		if (!new_buckets)
		{
			heistogram_free(result);
			return nullptr;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + result->capacity, 0, (max_capacity - result->capacity) * sizeof(FBucket));

		result->buckets = new_buckets;
		result->capacity = max_capacity;
	}

	// Merge buckets by adding counts
	for (uint16 i = 0; i < h1->capacity; i++)
	{
		result->buckets[i].count += h1->buckets[i].count;
	}

	for (uint16 i = 0; i < h2->capacity; i++)
	{
		result->buckets[i].count += h2->buckets[i].count;
	}

	// Update result metadata
	result->total_count = h1->total_count + h2->total_count;
	result->min = h1->min < h2->min ? h1->min : h2->min;
	result->max = h1->max > h2->max ? h1->max : h2->max;
	result->min_bucket_id = h1->min_bucket_id < h2->min_bucket_id ? h1->min_bucket_id : h2->min_bucket_id;

	return result;
}

// NEW: In-place merge of h2 into h1
static int heistogram_merge_inplace(FHeistogram* h1, const FHeistogram* h2)
{
	if (!h1 || !h2)
	{
		return 0;
	}

	// Expand h1 if needed to accommodate h2's buckets
	if (h2->capacity > h1->capacity)
	{
		FBucket* new_buckets = static_cast<FBucket*>(FMemory::Realloc(h1->buckets, h2->capacity * sizeof(FBucket)));
		if (!new_buckets)
		{
			return 0;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + h1->capacity, 0, (h2->capacity - h1->capacity) * sizeof(FBucket));

		h1->buckets = new_buckets;
		h1->capacity = h2->capacity;
	}

	// Merge buckets by adding counts
	for (uint16 i = 0; i < h2->capacity; i++)
	{
		h1->buckets[i].count += h2->buckets[i].count;
	}

	// Update h1 metadata
	h1->total_count += h2->total_count;
	if (h2->min < h1->min)
	{
		h1->min = h2->min;
	}
	if (h2->max > h1->max)
	{
		h1->max = h2->max;
	}
	if (h2->min_bucket_id < h1->min_bucket_id)
	{
		h1->min_bucket_id = h2->min_bucket_id;
	}

	return 1;
}

static double heistogram_percentile(const FHeistogram* h, double p)
{
	if (!h || p < 0 || p > 100)
	{
		return 0;
	}

	double target = ((100.0 - p) / 100.0) * h->total_count;
	uint64 cumsum = 0;
	double pos;
	uint64 min_val;
	uint64 max_val;
	for (int16_t i = h->capacity - 1; i >= 0; i--)
	{
		if (h->buckets[i].count > 0)
		{
			if (cumsum + h->buckets[i].count >= target)
			{
				pos = (target - cumsum) / static_cast<double>(h->buckets[i].count);
				min_val = get_bucket_min(i);
				max_val = get_bucket_max(min_val);

				if (max_val > h->max)
				{
					max_val = h->max;
				}
				if (min_val < h->min)
				{
					min_val = h->min;
				}

				return (max_val) - pos * (max_val - min_val);
			}
			cumsum += h->buckets[i].count;
		}
	}

	return h->min;
}

// Fixed heistogram_prank function
static double heistogram_prank(const FHeistogram* h, double value)
{
	if (!h || h->total_count == 0)
	{
		return 0;
	}
	if (value < h->min)
	{
		return 0;
	}
	if (value >= h->max)
	{
		return 100.0; // Fixed: was returning h->max
	}

	int16_t bid = get_bucket_id(value);
	if (bid >= h->capacity)
	{
		return 100.0;
	}

	uint64 cumsum = 0;

	// Count all buckets below the target bucket
	for (int16_t i = 0; i < bid; i++)
	{
		cumsum += h->buckets[i].count;
	}

	// Calculate position within the target bucket
	uint64 min_val = get_bucket_min(bid);
	uint64 max_val = get_bucket_max(min_val);

	// Clamp bucket bounds to actual histogram bounds
	if (max_val > h->max)
	{
		max_val = h->max;
	}
	if (min_val < h->min)
	{
		min_val = h->min;
	}

	double pos;
	if (max_val == min_val)
	{
		pos = 1.0; // All values in bucket are <= value
	}
	else
	{
		pos = (value - min_val) / static_cast<double>(max_val - min_val);
		if (pos > 1.0)
		{
			pos = 1.0; // Clamp to bucket bounds
		}
	}

	// Add the fraction of the target bucket
	cumsum += static_cast<uint64>(pos * h->buckets[bid].count);

	return 100.0 * cumsum / h->total_count;
}

// New function: count elements <= value
static uint64 heistogram_count_upto(const FHeistogram* h, uint64 value)
{
	if (!h || h->total_count == 0)
	{
		return 0;
	}
	if (value < h->min)
	{
		return 0;
	}
	if (value >= h->max)
	{
		return h->total_count;
	}

	int16_t bid = get_bucket_id(value);
	if (bid >= h->capacity)
	{
		return h->total_count;
	}

	uint64 cumsum = 0;

	// Count all buckets below the target bucket
	for (int16_t i = 0; i < bid; i++)
	{
		cumsum += h->buckets[i].count;
	}

	// Calculate position within the target bucket
	uint64 min_val = get_bucket_min(bid);
	uint64 max_val = get_bucket_max(min_val);

	// Clamp bucket bounds to actual histogram bounds
	if (max_val > h->max)
	{
		max_val = h->max;
	}
	if (min_val < h->min)
	{
		min_val = h->min;
	}

	double pos;
	if (max_val == min_val)
	{
		pos = 1.0; // All values in bucket are <= value
	}
	else
	{
		pos = static_cast<double>(value - min_val) / static_cast<double>(max_val - min_val);
		if (pos > 1.0)
		{
			pos = 1.0; // Clamp to bucket bounds
		}
	}

	// Add the fraction of the target bucket
	cumsum += static_cast<uint64>(pos * h->buckets[bid].count);

	return cumsum;
}

static inline void* heistogram_serialize(const FHeistogram* h, size_t* size)
{
	if (!h || !size)
	{
		return nullptr;
	}

	// Find the highest used bucket ID
	int16_t max_bucket_id = h->capacity - 1;

	while (max_bucket_id >= 0 && h->buckets[max_bucket_id].count == 0)
	{
		max_bucket_id--;
	}


	// Max varint size is 9 bytes, allocate maximum possible size
	size_t max_var_size = 9;
	// We store: error_margin, max_bucket_id, total_count, min, max
	size_t header_size = 5 * max_var_size;
	// For each bucket we store the count
	size_t bucket_size = max_var_size;
	size_t max_total_size = header_size + ((max_bucket_id - h->min_bucket_id + 1) * bucket_size);

	uint8* buffer = static_cast<uint8*>(FMemory::Malloc(max_total_size));
	if (!buffer)
	{
		return nullptr;
	}

	uint8* ptr = buffer;
	ptr += encode_varint(max_bucket_id - h->min_bucket_id + 1, ptr); // Number of buckets to store
	ptr += encode_varint(h->total_count, ptr);
	ptr += encode_varint(h->min, ptr);
	ptr += encode_varint(h->max - h->min, ptr);
	ptr += encode_varint(h->min_bucket_id, ptr);

	// Write buckets in reverse order (higher ids first)
	for (int16_t i = max_bucket_id; i >= h->min_bucket_id; i--)
	{
		ptr += encode_bucket(h->buckets[i].count, ptr);
	}
	// Calculate actual size used
	*size = ptr - buffer;

	// Reallocate to actual size
	buffer = static_cast<uint8*>(FMemory::Realloc(buffer, *size));
	return buffer;
}

static inline FHeistogram* heistogram_deserialize(const void* buffer, size_t size)
{
	if (!buffer)
	{
		return nullptr;
	}

	const uint8* ptr = static_cast<const uint8*>(buffer);
	uint16 bucket_count;
	uint64 total_count;
	uint64 min;
	uint64 max;
	uint16 min_bucket_id;

	// Read header
	size_t bytes_read = decode_header(ptr, &bucket_count, &total_count, &min, &max, &min_bucket_id);
	if (bytes_read == 0)
	{
		return nullptr;
	}

	ptr += bytes_read;

	uint16 max_bucket_id = min_bucket_id + bucket_count - 1;

	FHeistogram* h = heistogram_create();
	if (!h)
	{
		return nullptr;
	}

	// Ensure we have enough capacity
	if (max_bucket_id >= h->capacity)
	{
		FBucket* new_buckets = static_cast<FBucket*>(
			FMemory::Realloc(h->buckets, (max_bucket_id + 1) * sizeof(FBucket)));
		if (!new_buckets)
		{
			heistogram_free(h);
			return nullptr;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + h->capacity, 0, (max_bucket_id + 1 - h->capacity) * sizeof(FBucket));

		h->buckets = new_buckets;
		h->capacity = max_bucket_id + 1;
	}

	h->total_count = total_count;
	h->min = min;
	h->max = max;

	// Read buckets in reverse order
	uint64 count;
	for (int16_t i = max_bucket_id; i >= min_bucket_id; i--)
	{
		bytes_read = decode_bucket(ptr, &count);
		if (bytes_read == 0)
		{
			heistogram_free(h);
			return nullptr;
		}
		ptr += bytes_read;
		h->buckets[i].count = count;
	}
	for (int16_t i = 0; i < min_bucket_id && i < 16; i++)
	{
		h->buckets[i].count = 0;
	}
	return h;
}

/*********************************************
    FUNCTIONS OPERATING ON SERIALIZED DATA
**********************************************/

// Updated heistogram_percentile_serialized function
static double heistogram_percentile_serialized(const void* buffer, size_t size, double p)
{
	if (!buffer || size < 3)
	{
		return 0; // Minimum size check for header
	}

	const uint8* ptr = static_cast<const uint8*>(buffer);
	uint16 bucket_count;
	uint64 total_count;
	uint64 min;
	uint64 max;
	uint16 min_bucket_id;
	// Decode the header
	size_t bytes_read = decode_header(ptr, &bucket_count, &total_count, &min, &max, &min_bucket_id);
	if (bytes_read == 0)
	{
		return 0;
	}

	ptr += bytes_read;

	uint16 max_bucket_id = min_bucket_id + bucket_count - 1;

	double target = ((100.0 - p) / 100.0) * total_count;
	uint64 cumsum = 0;
	uint64 count;

	// Process buckets in reverse order (higher IDs first)
	for (int16_t i = max_bucket_id; i >= min_bucket_id; i--)
	{
		bytes_read = decode_bucket(ptr, &count);
		if (bytes_read == 0)
		{
			return 0;
		}
		ptr += bytes_read;

		if (count > 0 && cumsum + count >= target)
		{
			double pos = (target - cumsum) / static_cast<double>(count);
			uint64 min_val = get_bucket_min(i);
			uint64 max_val = get_bucket_max(min_val);
			if (max_val > max)
			{
				max_val = max;
			}
			if (min_val < min)
			{
				min_val = min;
			}
			return max_val - pos * (max_val - min_val);
		}
		cumsum += count;
	}

	return min;
}


// Fixed function to merge in-memory Heistogram with serialized Heistogram
static FHeistogram* heistogram_merge_serialized(FHeistogram* h, const void* buffer, size_t size)
{
	if (!h || !buffer || size < 3)
	{
		return nullptr;
	}

	const uint8* ptr = static_cast<const uint8*>(buffer);
	uint16 bucket_count;
	uint64 total_count;
	uint64 min;
	uint64 max;
	uint16 min_bucket_id;


	// Decode the header
	size_t bytes_read = decode_header(ptr, &bucket_count, &total_count, &min, &max, &min_bucket_id);
	if (bytes_read == 0)
	{
		return nullptr;
	}
	ptr += bytes_read;

	uint16 max_bucket_id = min_bucket_id + bucket_count - 1;

	// Create a new Heistogram for the result
	FHeistogram* result = heistogram_create();
	if (!result)
	{
		return nullptr;
	}

	// Find the maximum capacity needed
	uint16 new_capacity = h->capacity > (max_bucket_id + 1) ? h->capacity : (max_bucket_id + 1);

	// Resize result to accommodate all buckets
	if (new_capacity > result->capacity)
	{
		FBucket* new_buckets = static_cast<FBucket*>(FMemory::Realloc(result->buckets, new_capacity * sizeof(FBucket)));
		if (!new_buckets)
		{
			heistogram_free(result);
			return nullptr;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + result->capacity, 0, (new_capacity - result->capacity) * sizeof(FBucket));

		result->buckets = new_buckets;
		result->capacity = new_capacity;
	}

	// Copy counts from h
	for (uint16 i = 0; i < h->capacity; i++)
	{
		result->buckets[i].count += h->buckets[i].count;
	}

	// Add counts from serialized data
	uint64 count;
	for (int16_t i = max_bucket_id; i >= min_bucket_id; i--)
	{
		bytes_read = decode_bucket(ptr, &count);
		if (bytes_read == 0)
		{
			heistogram_free(result);
			return nullptr;
		}
		ptr += bytes_read;

		result->buckets[i].count += count;
	}

	// Update result metadata
	result->total_count = h->total_count + total_count;
	result->min = h->min < min ? h->min : min;
	result->max = h->max > max ? h->max : max;
	result->min_bucket_id = h->min_bucket_id < min_bucket_id ? h->min_bucket_id : min_bucket_id;

	return result;
}

// Fixed function to merge two serialized Heistograms
static FHeistogram* heistogram_merge_two_serialized(const void* buffer1, size_t size1, const void* buffer2,
                                                    size_t size2)
{
	if (!buffer1 || size1 < 3 || !buffer2 || size2 < 3)
	{
		return nullptr;
	}

	// Decode headers
	const uint8* ptr1 = static_cast<const uint8*>(buffer1);
	uint16 bucket_count1;
	uint64 total_count1;
	uint64 min1;
	uint64 max1;
	uint16 min_bucket_id1;

	size_t bytes_read1 = decode_header(ptr1, &bucket_count1, &total_count1, &min1, &max1, &min_bucket_id1);
	if (bytes_read1 == 0)
	{
		return nullptr;
	}
	ptr1 += bytes_read1;

	uint16 max_bucket_id1 = min_bucket_id1 + bucket_count1 - 1;

	const uint8* ptr2 = static_cast<const uint8*>(buffer2);
	uint16 bucket_count2;
	uint64 total_count2;
	uint64 min2;
	uint64 max2;
	uint16 min_bucket_id2;

	size_t bytes_read2 = decode_header(ptr2, &bucket_count2, &total_count2, &min2, &max2, &min_bucket_id2);
	if (bytes_read2 == 0)
	{
		return nullptr;
	}
	ptr2 += bytes_read2;

	uint16 max_bucket_id2 = min_bucket_id2 + bucket_count2 - 1;

	// Create a new Heistogram for the result
	FHeistogram* result = heistogram_create();
	if (!result)
	{
		return nullptr;
	}

	// Find the maximum capacity needed
	uint16 max_bucket_id = max_bucket_id1 > max_bucket_id2 ? max_bucket_id1 : max_bucket_id2;
	uint16 min_bucket_id = min_bucket_id1 < min_bucket_id2 ? min_bucket_id1 : min_bucket_id2;
	uint16 new_capacity = max_bucket_id + 1;

	// Resize result to accommodate all buckets
	if (new_capacity > result->capacity)
	{
		FBucket* new_buckets = static_cast<FBucket*>(FMemory::Realloc(result->buckets, new_capacity * sizeof(FBucket)));
		if (!new_buckets)
		{
			heistogram_free(result);
			return nullptr;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + result->capacity, 0, (new_capacity - result->capacity) * sizeof(FBucket));

		result->buckets = new_buckets;
		result->capacity = new_capacity;
	}

	// Read counts from first serialized Heistogram
	uint64 count;
	for (int16_t i = max_bucket_id1; i >= min_bucket_id1; i--)
	{
		bytes_read1 = decode_bucket(ptr1, &count);
		if (bytes_read1 == 0)
		{
			heistogram_free(result);
			return nullptr;
		}
		ptr1 += bytes_read1;
		result->buckets[i].count += count;
	}

	// Read counts from second serialized Heistogram
	for (int16_t i = max_bucket_id2; i >= min_bucket_id2; i--)
	{
		bytes_read2 = decode_bucket(ptr2, &count);
		if (bytes_read2 == 0)
		{
			heistogram_free(result);
			return nullptr;
		}
		ptr2 += bytes_read2;
		result->buckets[i].count += count;
	}

	// Update result metadata
	result->total_count = total_count1 + total_count2;
	result->min = min1 < min2 ? min1 : min2;
	result->max = max1 > max2 ? max1 : max2;
	result->min_bucket_id = min_bucket_id;

	return result;
}

// New function to merge serialized Heistogram into an existing in-memory Heistogram
static int heistogram_merge_inplace_serialized(FHeistogram* h, const void* buffer, size_t size)
{
	if (!h || !buffer || size < 3)
	{
		return 0;
	}

	const uint8* ptr = static_cast<const uint8*>(buffer);
	uint16 bucket_count;
	uint64 total_count;
	uint64 min;
	uint64 max;
	uint16 min_bucket_id;

	// Decode the header
	size_t bytes_read = decode_header(ptr, &bucket_count, &total_count, &min, &max, &min_bucket_id);
	if (bytes_read == 0)
	{
		return 0;
	}

	if (total_count == 0)
	{
		return 1; // we are merging with an empty heistgram
	}

	ptr += bytes_read;

	uint16 max_bucket_id = min_bucket_id + bucket_count - 1;


	// Expand h if needed to accommodate serialized Heistogram's buckets
	if (max_bucket_id >= h->capacity)
	{
		uint16 new_capacity = max_bucket_id + 1;
		FBucket* new_buckets = static_cast<FBucket*>(FMemory::Realloc(h->buckets, new_capacity * sizeof(FBucket)));
		if (!new_buckets)
		{
			return 0;
		}

		// Initialize new buckets to zero
		FMemory::Memset(new_buckets + h->capacity, 0, (new_capacity - h->capacity) * sizeof(FBucket));

		h->buckets = new_buckets;
		h->capacity = new_capacity;
	}

	// Add counts from serialized data
	uint64 count;
	for (int16_t i = max_bucket_id; i >= min_bucket_id; i--)
	{
		bytes_read = decode_bucket(ptr, &count);
		if (bytes_read == 0)
		{
			return 0;
		}
		ptr += bytes_read;

		h->buckets[i].count += count;
	}

	// Update h metadata
	h->total_count += total_count;
	if (min < h->min)
	{
		h->min = min;
	}
	if (max > h->max)
	{
		h->max = max;
	}
	if (min_bucket_id < h->min_bucket_id)
	{
		h->min_bucket_id = min_bucket_id;
	}

	return 1;
}

#endif /* HEISTOGRAM_H */
