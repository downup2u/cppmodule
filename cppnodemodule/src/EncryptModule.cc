#include <string>
#include <node.h>
#include <v8.h>
using namespace v8;
#include "EncryptWrap.h"



Handle<Value> getEncryptStringHex(const Arguments& args) {
  HandleScope scope;
  if (args.Length() != 2) {
	  ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
	  return scope.Close(Undefined());
  }
  if (!args[0]->IsString() || !args[1]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }
  v8::String::Utf8Value vsrc(args[0]->ToString());
  v8::String::Utf8Value vkey(args[1]->ToString());

  std::string src = std::string(*vsrc); 
  std::string key = std::string(*vkey);
  std::string sretdata ;
  sretdata = EncryptWrap::get_mutable_instance().getEncryptStringHex(src,key);
  //@输入参数:
  //1)类型名std::string,
  //2)类型json数据，可转换为protobuf类std::string
  //3)用户ID,int类型,登录后返回
  //@输出参数:返回的json数据，std::string（数据格式:typename|json
  Handle<Value> retValue = String::New(sretdata.c_str());
  return scope.Close(retValue);
}


Handle<Value> getDecryptStringHex(const Arguments& args) {
  HandleScope scope;
  if (args.Length() != 2) {
	  ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
	  return scope.Close(Undefined());
  }
  if (!args[0]->IsString() || !args[1]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }
  v8::String::Utf8Value vsrc(args[0]->ToString());
  v8::String::Utf8Value vkey(args[1]->ToString());

  std::string src = std::string(*vsrc); 
  std::string key = std::string(*vkey);
  std::string sretdata ;
  sretdata = EncryptWrap::get_mutable_instance().getDecryptStringHex(src,key);
  //@输入参数:
  //1)类型名std::string,
  //2)类型json数据，可转换为protobuf类std::string
  //3)用户ID,int类型,登录后返回
  //@输出参数:返回的json数据，std::string（数据格式:typename|json
  Handle<Value> retValue = String::New(sretdata.c_str());
  return scope.Close(retValue);
}


Handle<Value> encryptString(const Arguments& args) {
	HandleScope scope;
	if (args.Length() != 2) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsString() || !args[1]->IsString()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	v8::String::Utf8Value vsrc(args[0]->ToString());
	v8::String::Utf8Value vkey(args[1]->ToString());

	std::string src = std::string(*vsrc);
	std::string key = std::string(*vkey);
	std::string sretdata;
	sretdata = EncryptWrap::get_mutable_instance().encryptString(src, key);
	//@输入参数:
	//1)类型名std::string,
	//2)类型json数据，可转换为protobuf类std::string
	//3)用户ID,int类型,登录后返回
	//@输出参数:返回的json数据，std::string（数据格式:typename|json
	Handle<Value> retValue = String::New(sretdata.c_str());
	return scope.Close(retValue);
}

Handle<Value> decryptString(const Arguments& args) {
	HandleScope scope;
	if (args.Length() != 2) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsString() || !args[1]->IsString()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	v8::String::Utf8Value vsrc(args[0]->ToString());
	v8::String::Utf8Value vkey(args[1]->ToString());

	std::string src = std::string(*vsrc);
	std::string key = std::string(*vkey);
	std::string sretdata;
	sretdata = EncryptWrap::get_mutable_instance().decryptString(src, key);
	//@输入参数:
	//1)类型名std::string,
	//2)类型json数据，可转换为protobuf类std::string
	//3)用户ID,int类型,登录后返回
	//@输出参数:返回的json数据，std::string（数据格式:typename|json
	Handle<Value> retValue = String::New(sretdata.c_str());
	return scope.Close(retValue);
}


Handle<Value> hashedSn(const Arguments& args) {
	HandleScope scope;
	if (args.Length() != 1) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsString()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}
	v8::String::Utf8Value vSn(args[0]->ToString());

	std::string sSn = std::string(*vSn);
	std::string sretdata;
	sretdata = EncryptWrap::get_mutable_instance().hashedSn(sSn);
	//@输入参数:
	//1)类型名std::string,
	//2)类型json数据，可转换为protobuf类std::string
	//3)用户ID,int类型,登录后返回
	//@输出参数:返回的json数据，std::string（数据格式:typename|json
	Handle<Value> retValue = String::New(sretdata.c_str());
	return scope.Close(retValue);
}


Handle<Value> generateRandomSn(const Arguments& args) {
	HandleScope scope;
	if (args.Length() != 0) {
		ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}

	std::string sretdata;
	sretdata = EncryptWrap::get_mutable_instance().generateRandomSn();
	//@输入参数:
	//1)类型名std::string,
	//2)类型json数据，可转换为protobuf类std::string
	//3)用户ID,int类型,登录后返回
	//@输出参数:返回的json数据，std::string（数据格式:typename|json
	Handle<Value> retValue = String::New(sretdata.c_str());
	return scope.Close(retValue);
}



void init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("getEncryptStringHex"),
	  FunctionTemplate::New(getEncryptStringHex)->GetFunction());
  exports->Set(String::NewSymbol("getDecryptStringHex"),
	  FunctionTemplate::New(getDecryptStringHex)->GetFunction());
  exports->Set(String::NewSymbol("encryptString"),
	  FunctionTemplate::New(encryptString)->GetFunction());
  exports->Set(String::NewSymbol("decryptString"),
	  FunctionTemplate::New(decryptString)->GetFunction());
  exports->Set(String::NewSymbol("hashedSn"),
	  FunctionTemplate::New(hashedSn)->GetFunction());
  exports->Set(String::NewSymbol("generateRandomSn"),
	  FunctionTemplate::New(generateRandomSn)->GetFunction());
}

//#ifdef WINDOWS
//#ifdef _WIN64
//NODE_MODULE(DataModule64, init)
//#else
//NODE_MODULE(DataModule32, init)
//#endif
//#else
NODE_MODULE(EncryptModule32, init)
//#endif