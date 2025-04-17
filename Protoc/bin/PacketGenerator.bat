pushd %~dp0

protoc.exe --proto_path=./ --cpp_out=./ ./Enum.proto 
protoc.exe --proto_path=./ --cpp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto

protoc.exe -I=./ --csharp_out=./ ./Enum.proto 
protoc.exe -I=./ --csharp_out=./ ./Struct.proto 
protoc.exe -I=./ --csharp_out=./ ./Protocol.proto 

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Protocol.pb.h "../../GameClient"
XCOPY /Y Protocol.pb.cc "../../GameClient"
XCOPY /Y Struct.pb.h "../../GameClient"
XCOPY /Y Struct.pb.cc "../../GameClient"
XCOPY /Y Enum.pb.h "../../GameClient"
XCOPY /Y Enum.pb.cc "../../GameClient"

XCOPY /Y Protocol.pb.h "../../GameServer"
XCOPY /Y Protocol.pb.cc "../../GameServer"
XCOPY /Y Struct.pb.h "../../GameServer"
XCOPY /Y Struct.pb.cc "../../GameServer"
XCOPY /Y Enum.pb.h "../../GameServer"
XCOPY /Y Enum.pb.cc "../../GameServer"

XCOPY /Y Protocol.cs ..\..\..\Frame_GameClient\GameClient\Assets\Scripts\ProtoBuf\
XCOPY /Y Struct.cs   ..\..\..\Frame_GameClient\GameClient\Assets\Scripts\ProtoBuf\
XCOPY /Y Enum.cs ..\..\..\Frame_GameClient\GameClient\Assets\Scripts\ProtoBuf\