// Get number of elements in an array:
#define NELEM(array)	(sizeof(array) / sizeof(*(array)))

// Loop over an array of given size:
#define FOREACH_NELEM(array, nelem, iter) \
	for (__typeof__(*(array)) *iter = (array); \
		iter < (array) + (nelem); \
		iter++)

// Loop over an array of known size:
#define FOREACH(array, iter) \
	FOREACH_NELEM(array, NELEM(array), iter)
