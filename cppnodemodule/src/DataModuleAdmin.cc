#include <string>
#include "Async.h"
#include "DataWrapAdmin.h"
#include "mylib/util/utillanguage.h"
#include <glog.h>
//v8文档参考:http://izs.me/v8-docs/
Handle<Value> initDBPool(const Arguments& args) {
  HandleScope scope;	
  if (args.Length() != 1) {
	  ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
	  return scope.Close(Undefined());
  }
  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }
  v8::String::Utf8Value vXML(args[0]->ToString());  
  std::string sXML = std::string(*vXML);
  bool bSuc = DataWrapAdmin::get_mutable_instance().initDB(sXML);
  //@输入参数:连接池个数int,连接字符串std::string
  //@输出参数:是否成功bool
  //bool bsuc = 

  Handle<Value> retValue = v8::Boolean::New(bSuc);
  return scope.Close(retValue);
}

Handle<Value> addLanguage(const Arguments& args) {
	HandleScope scope;
	if (args.Length() != 2) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsString() || !args[1]->IsString()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	v8::String::Utf8Value vLanguage(args[0]->ToString());
	v8::String::Utf8Value vXMLPath(args[1]->ToString());

	std::string sLanguage = std::string(*vLanguage);
	std::string sXMLPath = std::string(*vXMLPath);
	UtilLanguage::get_mutable_instance().addLanguage(sLanguage, sXMLPath);

	Handle<Value> retValue = v8::Boolean::New(true);
	return scope.Close(retValue);
}

Handle<Value> setLogPath(const Arguments& args) {
	HandleScope scope;
	if (args.Length() != 1 && args.Length() != 2) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsString()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	int loglevel = 0;
	if (args.Length() == 2){
		loglevel = args[1]->Int32Value();
	}
	v8::String::Utf8Value vLogPath(args[0]->ToString());

	std::string sLogPath = std::string(*vLogPath);
	GlobalLog::initLogPath(sLogPath, loglevel);

	Handle<Value> retValue = v8::Boolean::New(true);
	return scope.Close(retValue);
}

void init(Handle<Object> exports) {
	exports->Set(String::NewSymbol("initDBPool"),
		FunctionTemplate::New(initDBPool)->GetFunction());
	exports->Set(String::NewSymbol("addLanguage"),
		FunctionTemplate::New(addLanguage)->GetFunction());
	exports->Set(String::NewSymbol("setLogPath"),
		FunctionTemplate::New(setLogPath)->GetFunction());
	exports->Set(String::NewSymbol("getDataWithSessionHexAsync"),
	  FunctionTemplate::New(getDataWithSessionHexAsync)->GetFunction());
	exports->Set(String::NewSymbol("getDataWithSessionJsonAsync"),
		FunctionTemplate::New(getDataWithSessionJsonAsync)->GetFunction());
}


NODE_MODULE(DataAdminModule32, init)

