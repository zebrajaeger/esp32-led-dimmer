let sliders = {};

// Options
sliders.defaultSliderOptions = {
    value: 500.0,
    min: 0,
    max: 4095,
    step: 1,

    paddingStart: 0.05,
    paddingEnd: 0.05,
    fixedHandle: false,
    flipped: true,

    ruler: {
        labels: {
            visible: false
        },
        tickMarks: {
            short: {
                visible: true,
                step: 128,
                pos: 0.2,
                size: 0.1
            },
            long: {
                visible: true,
                step: 512,
                pos: 0.15,
                size: 0.15
            }
        }
    },

    handle: {
        size: 0.1,
        zoom: 1.5,
        animation: 0
    }
};

sliders.optionCopy = () => {
    return JSON.parse(JSON.stringify(sliders.defaultSliderOptions));
};

sliders.initializing = false;
sliders.initAction = (cb) => {
    // prevent recursion of events
    if (sliders.initializing) {
        return
    }
    sliders.initializing = true;
    cb();
    sliders.initializing = false;
};

// move
sliders.moving = false;
sliders.ifNotMoving = (cb) => {
    if (!sliders.moving) {
        cb();
    }
};

sliders.moveAction = (cb) => {
    // prevent recursion of events
    if (sliders.moving) {
        return
    }
    sliders.moving = true;
    cb();
    sliders.moving = false;
};

// gettser/setter
sliders.setSliderValue = (id, v) => {
    if (typeof id === 'number') {
        $(".single-channel  input[type=range]").eq(id).rsSliderLens('option', 'value', v);
    } else if (id == 'single-odd') {
        $(".odd-channel input[type=range]").rsSliderLens('option', 'value', v);
    } else if (id == 'single-even') {
        $(".even-channel input[type=range]").rsSliderLens('option', 'value', v);
    } else if (id == 'single') {
        $(".single-channel input[type=range]").rsSliderLens('option', 'value', v);
    } else if (id == 'all') {
        $(".channel-all input[type=range]").rsSliderLens('option', 'value', v);
    } else if (id == 'even') {
        $(".channel-even input[type=range]").rsSliderLens('option', 'value', v);
    } else if (id == 'odd') {
        $(".channel-odd input[type=range]").rsSliderLens('option', 'value', v);
    }
};

sliders.getSliderValue = (id) => {
    if (typeof id === 'number') {
        return $(".single-channel  input[type=range]").eq(id).rsSliderLens('option', 'value');
    } else if (id == 'all') {
        return $(".channel-all input[type=range]").rsSliderLens('option', 'value');
    } else if (id == 'even') {
        return $(".channel-even input[type=range]").rsSliderLens('option', 'value');
    } else if (id == 'odd') {
        return $(".channel-odd input[type=range]").rsSliderLens('option', 'value');
    }
    return null;
};

sliders.each = (typ, cb) => {
    if (typ === 'single') {
        $(".single-channel input[type=range]").each(cb);
    } else if (typ === 'all') {
        $(".channel-all input[type=range]").each(cb);
    } else if (typ === 'odd') {
        $(".channel-odd input[type=range]").each(cb);
    } else if (typ === 'even') {
        $(".channel-even input[type=range]").each(cb);
    }
};

sliders.init = () => {
    sliders.initAction(() => {
        // single
        sliders.each('single', (i, o) => {
            let options = sliders.optionCopy();
            options['onChange'] = () => {
                sliders.moveAction(() => {
                    sliders.setSliderValue('all', 0);
                    if (i % 2 === 0) {
                        sliders.setSliderValue('odd', 0);
                    } else {
                        sliders.setSliderValue('even', 0);
                    }
                    sliders.change();
                });
            };
            $(o).rsSliderLens(options);
        });

        // all
        sliders.each('all', (i, o) => {
            let options = sliders.optionCopy();
            options['onChange'] = (event, value) => {
                sliders.moveAction(() => {
                    sliders.setSliderValue('single', value);
                    sliders.setSliderValue('odd', value);
                    sliders.setSliderValue('even', value);
                    sliders.change();
                });
            };
            $(o).rsSliderLens(options);
        });

        // odd
        sliders.each('odd', (i, o) => {
            let options = sliders.optionCopy();
            options['onChange'] = (event, value, isFirstHandle) => {
                sliders.moveAction(() => {
                    sliders.setSliderValue('all', 0);
                    sliders.setSliderValue('single-odd', value);
                    sliders.change();
                });
            };
            $(o).rsSliderLens(options);
        });

        $(".channel-even input[type=range]").each((i, o) => {
            let options = sliders.optionCopy();
            options['onChange'] = (event, value, isFirstHandle) => {
                sliders.moveAction(() => {
                    sliders.setSliderValue('all', 0);
                    sliders.setSliderValue('single-even', value);
                    sliders.change();
                });
            };
            $(o).rsSliderLens(options);
        });
    });
};

sliders.change = () => {
    if (!sliders.initializing) {
        if (sliders.onChange) {
            let values = [];
            sliders.each('single', (i, o) => {
                values.push($(o).rsSliderLens('option', 'value'));
            });
            sliders.onChange(values);
        }
    }
};

sliders.setChannels = (channelArray) => {
    let allSame = true;
    let allValue = null;

    let oddSame = true;
    let oddValue = null;

    let evenSame = true;
    let evenValue = null;

    sliders.moveAction(() => {
        sliders.each('single', (i, o) => {
            let v = channelArray[i];

            if (allValue === null) {
                allValue = v;
            } else {
                if (allValue !== v) {
                    allSame = false;
                }
            }

            if (i % 2 === 1) {
                if (evenValue === null) {
                    evenValue = v;
                } else {
                    if (evenValue !== v) {
                        evenSame = false;
                    }
                }
            } else {
                if (oddValue === null) {
                    oddValue = v;
                } else {
                    if (oddValue !== v) {
                        oddSame = false;
                    }
                }
            }

            sliders.setSliderValue(i, v);
        });

        if (allSame) {
            sliders.setSliderValue('all', allValue);
        } else {
            sliders.setSliderValue('all', 0);
        }

        if (oddSame) {
            sliders.setSliderValue('odd', oddValue);
        } else {
            sliders.setSliderValue('odd', 0);
        }

        if (evenSame) {
            sliders.setSliderValue('even', evenValue);
        } else {
            sliders.setSliderValue('even', 0);
        }
    })
};

sliders.onChange = null;
