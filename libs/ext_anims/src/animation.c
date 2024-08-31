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

#define DEBUG 1

#include <nt5emul/renderer_keyframe.h>
#include <nt5emul/renderer_animation.h>
#include <nt5emul/renderer_ease.h>
#include <stddef.h>
#if DEBUG == 1
#include <stdio.h>
#endif

void _ntRendererUpdateAnimation(struct renderer_animation *animation) {
    animation->valid = false;

    if (animation->count == 0) return;
    if (animation->keyframes == NULL) return;

    animation->valid = true;

    if (animation->linked_animation != NULL) {
        struct renderer_animation *anim = (struct renderer_animation *)animation->linked_animation;
        anim->delta = animation->delta;

        _ntRendererUpdateAnimation(anim);

        animation->completed = animation->completed_local && anim->completed_local;
    } else {
        animation->completed = animation->completed_local;
    }

    struct renderer_keyframe *selected = animation->keyframes + animation->current_keyframe;

    if (animation->current_keyframe < animation->count) {
        if (animation->itime >= selected->length) {
            animation->itime = 0;
            animation->current_keyframe++;
            animation->final_value = animation->starting_value + selected->ending_value;

#if DEBUG == 1
            printf("[%d] keyframe %d completed\n", animation->anim_id, animation->current_keyframe);
#endif

            if (animation->current_keyframe >= animation->count) {
                animation->completed_local = true;
#if DEBUG == 1
                printf("[%d] animation completed\n", animation->anim_id);
#endif
                return;
            }
        
            animation->starting_value += selected->ending_value;

            selected = animation->keyframes + animation->current_keyframe;
        }
    }

    static double (*easings[TOEnd])(double) = {
        _rendererLinear, _rendererInSine, _rendererOutSine,
        _rendererInOutSine, _rendererInQuad, _rendererOutQuad,
        _rendererInOutQuad, _rendererInCubic, _rendererOutCubic,
        _rendererInOutCubic, _rendererInQuart, _rendererOutQuart,
        _rendererInOutQuart, _rendererInQuint, _rendererOutQuint,
        _rendererInOutQuint, _rendererInExpo, _rendererOutExpo,
        _rendererInOutExpo, _rendererInCirc, _rendererOutCirc,
        _rendererInOutCirc, _rendererInBack, _rendererOutBack,
        _rendererInOutBack, _rendererInElastic, _rendererOutElastic,
        _rendererInOutElastic, _rendererInBounce, _rendererOutBounce,
        _rendererInOutBounce 
    };

    double _res = 0;
    double res = 0;

    if (animation->current_keyframe < animation->count) {
#if 0
        printf("[%d] processing keyframe %d\n", animation->anim_id, animation->current_keyframe);
#endif
        
        double (*selected_easing)(double) = easings[selected->easing];

        _res = selected_easing(animation->itime / selected->length);

        if (selected->easing == TOLinear) {
            _res = (double)1 - _res;
        }

        res = animation->starting_value + (selected->ending_value * _res);
    } else {
        res = animation->final_value;
    }

    animation->local_current_value = res;

    if (animation->influenced && animation->linked_animation != NULL) {
        struct renderer_animation *anim = (struct renderer_animation *)animation->linked_animation;

        res += anim->current_value;
    }

    animation->current_value = res;    

    animation->time += animation->delta;
    animation->itime += animation->delta;
}