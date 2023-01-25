// Header files
#include <cstring>
#include <limits>
#include <node_api.h>
#include <tuple>
#include <vector>

using namespace std;


// BLAKE2b namespace
namespace Blake2b {

	// Header files
	#include "./BLAKE2b-NPM-Package-master/main.cpp"
}


// Constants

// Operation failed
static napi_value OPERATION_FAILED;


// Function prototypes

// Compute
static napi_value compute(napi_env environment, napi_callback_info arguments);

// Uint8 array to buffer
static tuple<uint8_t *, size_t, bool> uint8ArrayToBuffer(napi_env environment, napi_value uint8Array);

// Buffer to uint8 array
static napi_value bufferToUint8Array(napi_env environment, uint8_t *data, size_t size);


// Main function

// Initialize module
NAPI_MODULE_INIT() {

	// Check if initializing operation failed failed
	if(napi_get_null(env, &OPERATION_FAILED) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}

	// Check if creating compute property failed
	napi_value temp;
	if(napi_create_function(env, nullptr, 0, compute, nullptr, &temp) != napi_ok || napi_set_named_property(env, exports, "compute", temp) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}
	
	// Check if creating operation failed property failed
	if(napi_set_named_property(env, exports, "OPERATION_FAILED", OPERATION_FAILED) != napi_ok) {
	
		// Return nothing
		return nullptr;
	}
	
	// Return exports
	return exports;
}


// Supporting function implementation

// Compute
napi_value compute(napi_env environment, napi_callback_info arguments) {

	// Check if not enough arguments were provided
	size_t argc = 3;
	vector<napi_value> argv(argc);
	if(napi_get_cb_info(environment, arguments, &argc, argv.data(), nullptr, nullptr) != napi_ok || argc != argv.size()) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting result size from arguments failed
	double resultSize;
	if(napi_get_value_double(environment, argv[0], &resultSize) != napi_ok || resultSize < 0 || resultSize > numeric_limits<vector<uint8_t>::size_type>::max()) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting input from arguments failed
	const tuple<uint8_t *, size_t, bool> input = uint8ArrayToBuffer(environment, argv[1]);
	if(!get<2>(input)) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if getting key from arguments failed
	const tuple<uint8_t *, size_t, bool> key = uint8ArrayToBuffer(environment, argv[2]);
	if(!get<2>(key)) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Check if computing BLAKE2b failed
	vector<uint8_t> result(resultSize);
	if(!Blake2b::compute(result.data(), result.size(), get<0>(input), get<1>(input), get<0>(key), get<1>(key))) {
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Return result as a uint8 array
	return bufferToUint8Array(environment, result.data(), result.size());
}

// Uint8 array to buffer
tuple<uint8_t *, size_t, bool> uint8ArrayToBuffer(napi_env environment, napi_value uint8Array) {

	// Check if uint8 array isn't a typed array
	bool isTypedArray;
	if(napi_is_typedarray(environment, uint8Array, &isTypedArray) != napi_ok || !isTypedArray) {
	
		// Return failure
		return {nullptr, 0, false};
	}
	
	// Check if uint8 array isn't a uint8 array
	napi_typedarray_type type;
	size_t size;
	uint8_t *data;
	if(napi_get_typedarray_info(environment, uint8Array, &type, &size, reinterpret_cast<void **>(&data), nullptr, nullptr) != napi_ok || type != napi_uint8_array) {
	
		// Return failure
		return {nullptr, 0, false};
	}
	
	// Return data and size
	return {data, size, true};
}

// Buffer to uint8 array
napi_value bufferToUint8Array(napi_env environment, uint8_t *data, size_t size) {

	// Check if creating array buffer failed
	uint8_t *arrayBufferData;
	napi_value arrayBuffer;
	if(napi_create_arraybuffer(environment, size, reinterpret_cast<void **>(&arrayBufferData), &arrayBuffer) != napi_ok) {
	
		// Clear data
		memset(data, 0, size);
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Copy data to array buffer
	memcpy(arrayBufferData, data, size);
	
	// Clear data
	memset(data, 0, size);
	
	// Check if creating uint8 array from array buffer failed
	napi_value uint8Array;
	if(napi_create_typedarray(environment, napi_uint8_array, size, arrayBuffer, 0, &uint8Array) != napi_ok) {
	
		// Clear array buffer
		memset(arrayBufferData, 0, size);
	
		// Return operation failed
		return OPERATION_FAILED;
	}
	
	// Return uint8 array
	return uint8Array;
}
