pushd %~dp0

protoc.exe --proto_path=./ --cpp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto
protoc.exe --proto_path=./ --csharp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --csharp_out=./ ./Protocol.proto

IF ERRORLEVEL 1 PAUSE


XCOPY /Y Protocol.cs ..\..\..\Frame_GameClient\GameClient\Assets\Scripts\Packet\
XCOPY /Y Struct.cs   ..\..\..\Frame_GameClient\GameClient\Assets\Scripts\Packet\

XCOPY /Y Protocol.pb.h ..\..\GameServer\
XCOPY /Y Protocol.pb.cc ..\..\GameServer\
XCOPY /Y Struct.pb.h   ..\..\GameServer\
XCOPY /Y Struct.pb.cc  ..\..\GameServer\