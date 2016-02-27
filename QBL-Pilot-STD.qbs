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
        "QBL",
        "Middleware/FatFS",
        "Middleware/mavlink",
        "Middleware/mavlink/common",
    ]


    files: [
        "Driver/*.c",
        "GCC/*.c",
        "GCC/*.s",
        "STD/Src/*.c",
        "STD/Inc/*.h",
        "STD/*.c",
        "STD/*.h",
        "User/*.*",
        "QBL/*.c",
        "Middleware/DSP/BasicMathFunctions/*.*",
        "Middleware/DSP/CommonTables/*.*",
        "Middleware/DSP/ComplexMathFunctions/*.*",
        "Middleware/DSP/ControllerFunctions/*.*",
        "Middleware/DSP/FastMathFunctions/*.*",
        "Middleware/DSP/FilteringFunctions/*.*",
        "Middleware/DSP/MatrixFunctions/*.*",
        "Middleware/DSP/StatisticsFunctions/*.*",
        "Middleware/DSP/SupportFunctions/*.*",
        "Middleware/DSP/TransformFunctions/*.*",
        "Middleware/FatFS/*.*",
    ]

    cpp.defines :[
        "USE_STDPERIPH_DRIVER",
        "STM32F40_41xxx",
        "ARM_MATH_CM4",
        "__FPU_PRESENT=1"
    ]

    cpp.linkerScripts:[
        "GCC/*.ld"
    ]

}
