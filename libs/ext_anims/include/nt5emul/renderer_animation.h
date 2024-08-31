/*
    nt5 -- Windows XP simulator.
    Copyright (C) 2024  Sergei Baigerov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Contact Sergei Baigerov -- @dogotrigger in Discord
*/

#pragma once

// #include <nt5emul/renderer_keyframe.h>


#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct renderer_keyframe;
struct renderer_animation {
    // keyframe array
    struct renderer_keyframe *keyframes;
    // amount of objects inside keyframes array
    unsigned int count;

    // currently processed keyframe
    unsigned int current_keyframe;

    // starting value to work with
    // modified when keyframe is finished
    double starting_value;

    // current value
    // it also can be incremented by current_value from linked animation
    // but this behaviour is disabled by default
    double current_value;

    // local current value
    // it is not affected by the linked animation
    double local_current_value;

    // value on animation end
    double final_value;

    // time spent on whole animation
    double time;

    // time spent on a currently processed keyframe
    double itime;

    // delta time for this animation
    double delta;

    // linked animation
    struct renderer_animation *linked_animation;

    // if this flag is set to true then current_value also would include current_value from the linked_animation
    bool influenced;

    // animation id, useful for debugging
    int anim_id;

    // indicates if animation has been completed or not
    bool completed;

    // indicates if animation has been completed locally or not
    bool completed_local;

    // indicates if animation is valid or not
    bool valid;
};

void _ntRendererUpdateAnimation(struct renderer_animation *animation);

// check if specific animation id exists inside the main node
bool _ntRendererAnimIdExists(struct renderer_animation *animation, int anim_id);
// if not found 'assert' would be called. child nodes also are gonna be checked
double _ntRendererGetAnimationResult(struct renderer_animation *animation, int anim_id);

void _ntRendererPrintAnimationTree(struct renderer_animation *animation);
void _ntRendererResetAnimTree();

#ifdef __cplusplus
}
#endif