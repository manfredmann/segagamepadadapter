var Process = require("qbs.Process")

function getInfo(libopencmDir, device, mode) {
    var proc = new Process()
    var args = []

    args.push('ld/devices.data')
    args.push(device)
    args.push(mode)

    proc.setWorkingDirectory(libopencmDir)
    proc.exec("scripts/genlink.py", args, true)
    return proc.readStdOut()
}

function getCPU(libopencmDir, device) {
    return getInfo(libopencmDir, device, 'CPU')
}

function getFamily(libopencmDir, device) {
    return getInfo(libopencmDir, device, 'FAMILY')
}

function getSubFamily(libopencmDir, device) {
    return getInfo(libopencmDir, device, 'SUBFAMILY')
}

function getFPU(libopencmDir, device) {
    var fpu_args = []
    var fpu_str = getInfo(libopencmDir, device, 'FPU')

    switch(fpu_str) {
        case 'soft':
            fpu_args.push('-msoft-float')
            break;
        case 'hard-fpv4-sp-d16':
            fpu_args.push('-mfloat-abi=hard')
            fpu_args.push('-mfpu=fpv4-sp-d16')
            break;
        case 'hard-fpv5-sp-d16':
            fpu_args.push('-mfloat-abi=hard')
            fpu_args.push('-mfpu=fpv5-sp-d16')
            break;
    }
    return fpu_args
}

function getLD(libopencmDir, device) {
    var ld_str = getInfo(libopencmDir, device, 'DEFS')
    var ld_lst = ld_str.split(" ")

    return ld_lst
}

function getCPPFLAGS(libopencmDir, device) {
    var cppflags_str = getInfo(libopencmDir, device, 'CPPFLAGS')
    var cppflags_lst = cppflags_str.split(" ")

    return cppflags_lst
}
