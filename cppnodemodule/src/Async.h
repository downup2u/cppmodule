#ifndef _ASYNC_INCLUDE
#define _ASYNC_INCLUDE
#include <node.h>
#include <v8.h>
using namespace v8;
Handle<Value> getDataWithSessionAsync(const Arguments& args,bool isHex);
Handle<Value> getDataWithSessionHexAsync(const Arguments& args);
Handle<Value> getDataWithSessionJsonAsync(const Arguments& args);
#endif