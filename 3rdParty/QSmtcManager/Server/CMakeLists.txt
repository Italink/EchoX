cmake_minimum_required(VERSION 3.12)

project(QSmtcServer CSharp)

add_executable(QSmtcServer
	Program.cs
)

#set_property(TARGET QSmtcServer PROPERTY WIN32_EXECUTABLE TRUE)

set_property(TARGET QSmtcServer
    PROPERTY VS_PACKAGE_REFERENCES "NPSMLib_*" "Newtonsoft.Json_*"
)

set_property(TARGET QSmtcServer PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "System"
    "System.Core"
    "System.Data"
    "System.Drawing"
)
