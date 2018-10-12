import qbs
import 'libopencm.js' as LibCM

Project {
    name: "segagamepadadapter"

    property string     device:     "STM32F103RBT6"
    property string     libopencm:  sourceDirectory + '/lib/libopencm3'
    property string     cpu:        cpuValidator.cpu
    property string     family:     familyValidator.family
    property string     subFamily:  subFamilyValidator.subFamily
    property stringList fpu:        fpuValidator.fpu
    property stringList defs:       defsValidator.defs
    property stringList cppflags:   cppflagsValidator.cppflags


    Probe {
        id: cpuValidator
        property string libopencmDir: libopencm
        property string dev: device
        property string cpu
        configure: {
            cpu = LibCM.getCPU(libopencmDir, dev)
            found = true;
        }
    }

    Probe {
        id: familyValidator
        property string libopencmDir: libopencm
        property string dev: device
        property string family
        configure: {
            family = LibCM.getFamily(libopencmDir, dev)
            found = true;
        }
    }

    Probe {
        id: subFamilyValidator
        property string libopencmDir: libopencm
        property string dev: device
        property string subFamily
        configure: {
            subFamily = LibCM.getSubFamily(libopencmDir, dev)
            found = true;
        }
    }

    Probe {
        id: fpuValidator
        property string libopencmDir: libopencm
        property string dev: device
        property stringList fpu
        configure: {
            fpu = LibCM.getFPU(libopencmDir, dev)
            found = true;
        }
    }

    Probe {
        id: defsValidator
        property string libopencmDir: libopencm
        property string dev: device
        property stringList defs
        configure: {
            defs = LibCM.getLD(libopencmDir, dev)
            found = true;
        }
    }

    Probe {
        id: cppflagsValidator
        property string libopencmDir: libopencm
        property string dev: device
        property stringList cppflags
        configure: {
            cppflags = LibCM.getCPPFLAGS(libopencmDir, dev)
            found = true;
        }
    }

    Product {
        name: "mcu"
        type: "bin"

        Depends { name: "cpp" }

        cpp.positionIndependentCode: false
        cpp.optimization: "small"
        cpp.executableSuffix: ".elf"

        cpp.includePaths: [
            project.libopencm + '/include/',
        ]

        cpp.libraryPaths: [
            project.libopencm + '/lib/',
        ]

        cpp.defines: [
            project.family.toUpperCase(),
            project.subFamily.toUpperCase(),
            project.device,
        ]

        cpp.commonCompilerFlags: [
            "-std=c99",
            "-mcpu=" + project.cpu,
            "-mthumb",
            "-Wextra",
            "-Wshadow",
            "-Wimplicit-function-declaration",
            "-Wredundant-decls",
            "-fno-common",
            "-ffunction-sections",
            "-fdata-sections",
            "-MD",
            "-Wall",
            "-Wundef",
         ].concat(project.fpu);

        cpp.cFlags: [];
        cpp.cxxFlags: [];

        cpp.linkerFlags: [
            "--static",
            "-lc",
            "-nostartfiles",
            "-mcpu=" + project.cpu,
            "-mthumb",
            "-Wl,-Map=" + project.buildDirectory + "/" + project.name + ".map",
            "-Wl,-gc-sections",
        ].concat(project.fpu);

        Group {
            name: "sources"
            files: [
                "src/*.c",
                "src/*.h",
            ]
        }

        Rule {
            multiplex: true
            Artifact {
                alwaysUpdated: true
                fileTags: [ 'ld' ]
                filePath: 'generated.' + project.device + '.ld'
            }

            prepare: {
                var args = [];

                args.push("-E")
                args.push("-mcpu=" + project.cpu)
                args.push("-mthumb")

                args = args.concat(project.fpu)
                args = args.concat(project.defs)

                args.push("-P")
                args.push("-E")
                args.push(project.sourceDirectory + "/lib/libopencm3/ld/linker.ld.S")
                args.push("-o")
                args.push(output.filePath)

                var cmd = new Command(product.cpp.compilerPath, args);
                cmd.description = 'generate linker script: ' + output.filePath;
                cmd.highlight = 'linker'

                return [cmd];
            }
        }
        Rule {
            multiplex: true
            inputs: ['ld', 'obj']
            Artifact {
                fileTags: ['elf']
                filePath: project.name + '.elf'
            }

            prepare: {
                var args = [];
                args = args.concat(product.cpp.linkerFlags)

                for (i in inputs["ld"])
                    args.push('-T' + inputs["ld"][i].filePath)

                for (i in product.cpp.libraryPaths) {
                    args.push('-L' + product.cpp.libraryPaths[i])
                }

                for (i = 0; i < inputs["obj"].length; i++)
                    args.push(inputs["obj"][i].filePath);

                args.push('-lopencm3_' + project.family);

                args.push("-Wl,--start-group")
                args.push("-lc")
                args.push("-lgcc")
                args.push("-lnosys")
                args.push("-Wl,--end-group")

                args.push('-o');
                args.push(output.filePath);

                var cmd = new Command(product.cpp.compilerPath, args)
                cmd.description = 'linking ' + project.name
                cmd.highlight = 'linker'
                return cmd;
            }
        }

        Rule {
            multiplex: true
            inputs: ['elf']

            Artifact {
                fileTags: ['bin']
                filePath: project.name + '.bin'
            }

            prepare: {
                var args = [];

                args.push("-O")
                args.push("binary")

                for (i in inputs["elf"]) {
                    args.push(inputs["elf"][i].filePath)
                }

                args.push(output.filePath);

                var cmd = new Command(product.cpp.objcopyPath, args)
                cmd.description = 'ELF to BIN ' + project.name
                cmd.highlight = 'linker'
                return cmd;
            }
        }
    }
}
