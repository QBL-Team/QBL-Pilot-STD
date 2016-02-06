import qbs
import CortexMProduct

CortexMProduct{
    useFPU: true
    type: ["application","size"]

    cpp.includePaths:[
        "Driver",
        "CMSIS/Include",
        "STD/Inc",
        "STD",
        "QBL"
    ]


    files: [
        "Driver/*.c",
        "GCC/*.c",
        "GCC/*.s",
        "STD/Src/*.c",
        "STD/*.c",
        "User/*.c",
        "QBL/*.c"
    ]

    cpp.defines :[
        "USE_STDPERIPH_DRIVER",
        "STM32F40_41xxx"
    ]

    cpp.linkerScripts:[
        "GCC/*.ld"
    ]

}
