const utils = {};

utils.init = () => {
    // nothing to do
};

// thx to https://gist.github.com/jed/982883
utils.hex = function () {
    let hex = [];

    for (let i = 0; i < 256; i++) {
        hex[i] = (i < 16 ? '0' : '') + (i).toString(16);
    }
    return hex;
}();

utils.uuid = () => {
    let r = crypto.getRandomValues(new Uint8Array(16));

    r[6] = r[6] & 0x0f | 0x40;
    r[8] = r[8] & 0x3f | 0x80;

    return (
        utils.hex[r[0]] +
        utils.hex[r[1]] +
        utils.hex[r[2]] +
        utils.hex[r[3]] +
        "-" +
        utils.hex[r[4]] +
        utils.hex[r[5]] +
        "-" +
        utils.hex[r[6]] +
        utils.hex[r[7]] +
        "-" +
        utils.hex[r[8]] +
        utils.hex[r[9]] +
        "-" +
        utils.hex[r[10]] +
        utils.hex[r[11]] +
        utils.hex[r[12]] +
        utils.hex[r[13]] +
        utils.hex[r[14]] +
        utils.hex[r[15]]
    );
};
