// https://www.mikrocontroller.net/articles/LED-Fading

const offset = 150;
const pwmResoluteion = 4096;
const steps = 256;

const y0 = calc(0, offset);
const yn = calc(steps - 1, offset);

pwm();

function pwm() {
    console.log(`// steps: ${steps}, offset: ${offset}, pwmResolution: ${pwmResoluteion}`);
    console.log('const uint16_t pwmtable_16[256] PROGMEM = {');
    let line = '';
    const valuesPerLine = 16;
    count = 0;
    for (let i = 0; i < steps; ++i) {
        ++count;
        const f = 1 - (i / steps);
        const y = calc(i, offset) - (y0 * f);
        line += y.toFixed()
        // line += i;
        if (i !== steps - 1) {
            line += ', '
        }
        if (count == valuesPerLine) {
            count = 0;
            console.log(line);
            line = '';
        }
    }
    if (line.length > 0) {
        console.log(line);
    }
    console.log('};');
}

function calc(i, off) {
    const x = i + off;
    const last = steps + off;
    return Math.pow(2, Math.log2(pwmResoluteion - 1) * (x + 1) / (last));
}