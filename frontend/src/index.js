sliders = {
    defaultSliderOptions: {
        value: 500.0,
        min: -1,
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
    },

    moving: false,

    init: () => {
        sliders.moving = true;
        console.log("INIT", "START");
        $(".single-channel input[type=range]").each((i, o) => {
            let singleSliderOptions = JSON.parse(JSON.stringify(sliders.defaultSliderOptions));
            singleSliderOptions['onChange'] = (event, value, isFirstHandle) => {
                console.log("SINGLE" + i, sliders.moving);
                if (!sliders.moving) {
                    sliders.moving = true;
                    $(".channel-all input[type=range]").rsSliderLens('option', 'value', 0);
                    sliders.moving = false;
                }
            };
            sliders['s' + i] = $(o).rsSliderLens(singleSliderOptions);
        });

        $(".channel-all input[type=range]").each((i, o) => {
            let singleSliderOptions = JSON.parse(JSON.stringify(sliders.defaultSliderOptions));
            singleSliderOptions['onChange'] = (event, value, isFirstHandle) => {
                console.log('ALL, sliders.moving');
                if (!sliders.moving) {
                    sliders.moving = true;
                    $(".single-channel input[type=range]").rsSliderLens('option', 'value', value);
                    $(".channel-odd input[type=range]").rsSliderLens('option', 'value', value);
                    $(".channel-even input[type=range]").rsSliderLens('option', 'value', value);
                    sliders.moving = false;
                }
            };
            sliders['all'] = $(o).rsSliderLens(singleSliderOptions);
        });

        $(".channel-odd input[type=range]").each((i, o) => {
            let singleSliderOptions = JSON.parse(JSON.stringify(sliders.defaultSliderOptions));
            singleSliderOptions['onChange'] = (event, value, isFirstHandle) => {
                if (!sliders.moving) {
                    sliders.moving = true;
                    $(".channel-all input[type=range]").rsSliderLens('option', 'value', 0);
                    $(".odd-channel input[type=range]").rsSliderLens('option', 'value', value);
                    sliders.moving = false;
                }

            };
            sliders['all'] = $(o).rsSliderLens(singleSliderOptions);
        });

        $(".channel-even input[type=range]").each((i, o) => {
            let singleSliderOptions = JSON.parse(JSON.stringify(sliders.defaultSliderOptions));
            singleSliderOptions['onChange'] = (event, value, isFirstHandle) => {
                if (!sliders.moving) {
                    sliders.moving = true;
                    $(".channel-all input[type=range]").rsSliderLens('option', 'value', 0);
                    $(".even-channel input[type=range]").rsSliderLens('option', 'value', value);
                    sliders.moving = false;
                }

            };
            sliders['all'] = $(o).rsSliderLens(singleSliderOptions);
        });
        console.log("INIT", "END");
        sliders.moving = false;
    }
};

$(document).ready(function () {
    sliders.init();
});
