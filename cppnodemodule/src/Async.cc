#include "../include/Async.h"
#include "../include/DataWrap.h"
#include <string>
// libuv allows us to pass around a pointer to an arbitrary
// object when running asynchronous functions. We create a
// data structure to hold the data we need during and after
// the async work.
typedef struct AsyncData {
	std::string sInTypeName;       //param1
	std::string sInHexData;		   //param2
	std::string sSessionHexData;   //param3
	Persistent<Function> callback; //callback function
	bool bSuccess;				   //param1 is success
	std::string sOutTypeName;      //param2
	std::string sOutHexData;       //param3
} AsyncData;


// Function to execute inside the worker-thread.
// It is not safe to access V8, or V8 data structures
// here, so everything we need for input and output
// should go on our req->data object.
void AsyncWork(uv_work_t *req) {
	// fetch our data structure
	AsyncData *asyncData = (AsyncData *)req->data;
	asyncData->bSuccess = DataWrap::get_mutable_instance().getDataMsg(asyncData->sInTypeName, asyncData->sInHexData,
		asyncData->sSessionHexData, asyncData->sOutTypeName, asyncData->sOutHexData);
}


// Function to execute when the async work is complete
// this function will be run inside the main event loop
// so it is safe to use V8 again
void AsyncAfter(uv_work_t *req) {
	HandleScope scope;

	// fetch our data structure
	AsyncData *asyncData = (AsyncData *)req->data;
	// create an arguments array for the callback
	Handle<Value> argv[] = {
		v8::Boolean::New(asyncData->bSuccess),
		String::New(asyncData->sOutTypeName.c_str()),
		String::New(asyncData->sOutHexData.c_str()),
		String::New(asyncData->sSessionHexData.c_str()),
	};

	// surround in a try/catch for safety
	TryCatch try_catch;
	// execute the callback function
	asyncData->callback->Call(Context::GetCurrent()->Global(), 4, argv);
	if (try_catch.HasCaught())
		node::FatalException(try_catch);

	// dispose the Persistent handle so the callback
	// function can be garbage-collected
	asyncData->callback.Dispose();
	// clean up any memory we allocated
	delete asyncData;
	delete req;
}

Handle<Value> getDataWithSessionHexAsync(const Arguments& args)
{
	HandleScope scope;
	if (args.Length() != 4) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsFunction()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	v8::String::Utf8Value vtypename(args[0]->ToString());
	v8::String::Utf8Value vhexdata(args[1]->ToString());
	v8::String::Utf8Value vsessiondata(args[2]->ToString());
	std::string stypename = std::string(*vtypename);
	std::string shexdata = std::string(*vhexdata);
	std::string ssessiondata = std::string(*vsessiondata);

	// create an async work token
	uv_work_t *req = new uv_work_t;
	AsyncData *asyncData = new AsyncData;
	req->data = asyncData;

	// expect a number as the first argument
	asyncData->sInTypeName = stypename;
	asyncData->sInHexData = shexdata;
	asyncData->sSessionHexData = ssessiondata;
	// expect a function as the second argument
	// we create a Persistent reference to it so
	// it won't be garbage-collected
	asyncData->callback = Persistent<Function>::New(
		Local<Function>::Cast(args[3]));

	// pass the work token to libuv to be run when a
	// worker-thread is available to
	uv_queue_work(
		uv_default_loop(),
		req,                          // work token
		AsyncWork,                    // work function
		(uv_after_work_cb)AsyncAfter  // function to run when complete
		);

	return scope.Close(Undefined());
}