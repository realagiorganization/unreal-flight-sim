using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealFlightSimEditorTarget : TargetRules
{
    public UnrealFlightSimEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "UnrealFlightSim" });
    }
}
