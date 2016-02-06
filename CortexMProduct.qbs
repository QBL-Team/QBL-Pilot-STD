import qbs
import qbs.FileInfo

Product {
    property string core: "cortex-m4"
    property bool useFPU: false
    Depends { name: "cpp" }


    type: "application"

    consoleApplication : true
    cpp.positionIndependentCode : false

    cpp.cxxFlags:["-std=c++11"]

    cpp.cFlags:["-std=c11"]


    cpp.warningLevel:"all"


    Properties {
        condition: useFPU == false


        cpp.commonCompilerFlags: [
            "-mcpu=" + core,
            "-mthumb",
            "-flto",
        ]

        cpp.linkerFlags: [
            "-mcpu=" + core,
            "-mthumb",
            "-Wl,--gc-sections",
            "-flto",
            "-mabi=aapcs",
            "--specs=nano.specs",
            "--specs=nosys.specs",
            "-fdata-sections",
            "-ffunction-sections"
        ]
    }


    Properties {
        condition: useFPU == true


        cpp.commonCompilerFlags: [
            "-mcpu=" + core,
            "-mthumb",
            "-flto",
            "-mfpu=fpv4-sp-d16",
            "-mfloat-abi=hard"
        ]

        cpp.linkerFlags: [
            "-mcpu=" + core,
            "-mthumb",
            "-Wl,--gc-sections",
            "-flto",
            "-mabi=aapcs",
            "--specs=nano.specs",
            "--specs=nosys.specs",
            "-fdata-sections",
            "-ffunction-sections",
            "-mfpu=fpv4-sp-d16",
            "-mfloat-abi=hard"
        ]
    }




    Rule{
        id:size
        inputs: "application"
        Artifact {
            fileTags: "size"
            filePath: "-"
        }
        prepare: {
            var args = [input.filePath];
            var cmd = new Command("arm-none-eabi-size",args);
            cmd.description = "File size:"+FileInfo.fileName(input.filePath);
            cmd.highlight = "linker";
            return cmd;
        }
    }
}
