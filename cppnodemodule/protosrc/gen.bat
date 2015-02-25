protoc.exe -I=./ --cpp_out=../gcomm --java_out=../../../bin ./comm.proto
protoc.exe -I=./ --cpp_out=../gcomm ./thirdinterface.proto
protoc.exe -I=./ --cpp_out=../gcomm ./session.proto
protoc.exe -I=./ --cpp_out=../gcomm ./admin.proto
COPY /Y .\\*.proto ..\\..\\..\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap
COPY /Y ..\\gcomm\\comm.pb.* ..\\..\\..\\bin\\
COPY /Y .\\*.proto  ..\\..\\..\\src\\web\\node_modules\\iteasynodewrap
COPY /Y .\\*.proto ..\\..\\..\\src\\services\\itwebservice\\node_modules\\iteasynodewarp
COPY /Y ..\\gcomm\\comm.pb.* ..\\..\\..\\src\\mobile\\cppphonemodule\\
pause