#pragma once

// value tweak object

#define GEN_TWEAK(tweak) TOIn##tweak, TOOut##tweak, TOInOut##tweak

#ifdef __cplusplus
extern "C" {
#endif

typedef enum renderer_tweak_type {
    TOLinear = 0,       // in0  out0  inout0

    GEN_TWEAK(Sine),    // in1  out2  inout3
    GEN_TWEAK(Quad),    // in4  out5  inout6
    GEN_TWEAK(Cubic),   // in7  out8  inout9
    GEN_TWEAK(Quart),   // in10 out11 inout12
    GEN_TWEAK(Quint),   // in13 out14 inout15
    GEN_TWEAK(Expo),    // in16 out17 inout18
    GEN_TWEAK(Circ),    // in19 out20 inout21
    GEN_TWEAK(Back),    // in22 out23 inout24
    GEN_TWEAK(Elastic), // in25 out26 inout27
    GEN_TWEAK(Bounce),  // in28 out29 inout30

    TOEnd
} renderer_tweak_type;

#undef GEN_TWEAK

struct renderer_keyframe {
    // length in seconds
    double length;

    // ending_value is the relative value. starting_value is incremented by ending_value
    // starting value does not directly exists inside a keyframe. starting_value is the resulting movement value from the previous keyframe
    double ending_value;

    renderer_tweak_type easing;
};

#ifdef __cplusplus
}
#endif