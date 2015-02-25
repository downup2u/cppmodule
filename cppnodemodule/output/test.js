var os = require('os');
var ostype = os.type();

var ProtoBuf = require('protobufjs');
var path = require('path');

var logflag = 1;
console.log(ostype);
if (ostype == "Windows_NT"){
	//test 3i
	dataModule3I = require(path.join(__dirname,"win","Data3IModule32.node"));
	builder3i = ProtoBuf.loadProtoFile(path.join(__dirname,"thirdinterface.proto"));
	ProtoBuf.loadProtoFile(path.join(__dirname,"session.proto"), builder3i);

	var xmlpath = path.join(__dirname,"db","dbinfo.xml");
	console.log(xmlpath);
	
	initdbret = dataModule3I.initDBPool(xmlpath);
	dataModule3I.setLogPath(path.join(__dirname,"logs","Task3I"),logflag);
	dataModule3I.addLanguage("chinese",path.join(__dirname,"language","chinese.xml"));
	dataModule3I.addLanguage("english",path.join(__dirname,"language","english.xml"));
	
	//test admin
	dataModuleAdmin = require(path.join(__dirname,"win","DataAdminModule32.node"));
	builderadmin = ProtoBuf.loadProtoFile(path.join(__dirname,"admin.proto"));
	ProtoBuf.loadProtoFile(path.join(__dirname,"session.proto"), builderadmin);
	
	initdbret = dataModuleAdmin.initDBPool(xmlpath);
	dataModuleAdmin.setLogPath(path.join(__dirname,"logs","TaskAdmin"),logflag);
	dataModuleAdmin.addLanguage("chinese",path.join(__dirname,"language","chinese.xml"));
	dataModuleAdmin.addLanguage("english",path.join(__dirname,"language","english.xml"));
	
	//test comm
	dataModuleComm = require(path.join(__dirname,"win","DataCommModule32.node"));
	buildercomm = ProtoBuf.loadProtoFile(path.join(__dirname,"comm.proto"));
	ProtoBuf.loadProtoFile(path.join(__dirname,"session.proto"), buildercomm);

	initdbret = dataModuleComm.initDBPool(xmlpath);
	dataModuleComm.setLogPath(path.join(__dirname,"logs","TaskComm"),logflag);
	dataModuleComm.addLanguage("chinese",path.join(__dirname,"language","chinese.xml"));
	dataModuleComm.addLanguage("english",path.join(__dirname,"language","english.xml"));
}
else{
	//test 3i
	dataModule3I = require(path.join(__dirname,"linux","Data3IModule32.node"));
	builder3i = ProtoBuf.loadProtoFile(path.join(__dirname,"thirdinterface.proto"));
	ProtoBuf.loadProtoFile(path.join(__dirname,"session.proto"), builder3i);

	var xmlpath = path.join(__dirname,"db","dbinfo.xml");
	console.log(xmlpath);
	
	initdbret = dataModule3I.initDBPool(xmlpath);
	dataModule3I.setLogPath(path.join(__dirname,"logs","Task3I"),logflag);
	dataModule3I.addLanguage("chinese",path.join(__dirname,"language","chinese.xml"));
	dataModule3I.addLanguage("english",path.join(__dirname,"language","english.xml"));
	
	//test admin
	dataModuleAdmin = require(path.join(__dirname,"linux","DataAdminModule32.node"));
	builderadmin = ProtoBuf.loadProtoFile(path.join(__dirname,"admin.proto"));
	ProtoBuf.loadProtoFile(path.join(__dirname,"session.proto"), builderadmin);
	
	initdbret = dataModuleAdmin.initDBPool(xmlpath);
	dataModuleAdmin.setLogPath(path.join(__dirname,"logs","TaskAdmin"),logflag);
	dataModuleAdmin.addLanguage("chinese",path.join(__dirname,"language","chinese.xml"));
	dataModuleAdmin.addLanguage("english",path.join(__dirname,"language","english.xml"));
	
	//test comm
	dataModuleComm = require(path.join(__dirname,"linux","DataCommModule32.node"));
	buildercomm = ProtoBuf.loadProtoFile(path.join(__dirname,"comm.proto"));
	ProtoBuf.loadProtoFile(path.join(__dirname,"session.proto"), buildercomm);

	initdbret = dataModuleComm.initDBPool(xmlpath);
	dataModuleComm.setLogPath(path.join(__dirname,"logs","TaskComm"),logflag);
	dataModuleComm.addLanguage("chinese",path.join(__dirname,"language","chinese.xml"));
	dataModuleComm.addLanguage("english",path.join(__dirname,"language","english.xml"));
}