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
#include <stdio.h>
#include <string.h>

static char __ntRendererAnimTreeText[64] = {};

void _ntRendererPrintAnimationTree(struct renderer_animation *animation) {
    if (animation == NULL) return;

    size_t tree_level_idx = strlen(__ntRendererAnimTreeText);

    printf("%sbegin anim_id %d:\n", __ntRendererAnimTreeText, animation->anim_id);

    if (!animation->valid) {
        printf("%s #invalid animation#\n", __ntRendererAnimTreeText);

        if (tree_level_idx != 0) {
            __ntRendererAnimTreeText[--tree_level_idx] = 0;
        }

        return;
    }

    printf("%s - initial value: %f\n", __ntRendererAnimTreeText, (float)animation->starting_value);
    printf("%s - current value: %f (local: %f)\n", __ntRendererAnimTreeText, (float)animation->current_value, (float)animation->local_current_value);
    printf("%s - final   value: %f\n", __ntRendererAnimTreeText, (float)animation->final_value);

    printf("\n%s - time spent: %f\n", __ntRendererAnimTreeText, (float)animation->time);
    printf("%s - time spent on keyframe: %f\n", __ntRendererAnimTreeText, (float)animation->itime);
    printf("%s - delta time: %f\n", __ntRendererAnimTreeText, (float)animation->delta);

    printf("\n%s - completed: %d (local %d)\n", __ntRendererAnimTreeText, animation->completed, animation->completed_local);

    printf("\n%s - current keyframe: %d\n", __ntRendererAnimTreeText, animation->current_keyframe);
    printf("%s - keyframes (%d):\n", __ntRendererAnimTreeText, animation->count);

    for (unsigned int i = 0; i < animation->count; i++) {
        struct renderer_keyframe keyframe = animation->keyframes[i];

        printf("%s  %d: (v)%f; (e)%d; (l)%f\n", __ntRendererAnimTreeText, i, (float)keyframe.ending_value, keyframe.easing, (float)keyframe.length);
    }

    if (animation->linked_animation != NULL) {
        printf("\n%s - linked animation:\n", __ntRendererAnimTreeText);
        
        if (tree_level_idx == 63) {
            printf("%s   cannot be displayed: too big tree\n", __ntRendererAnimTreeText);
        } else {
            __ntRendererAnimTreeText[tree_level_idx + 0] = ' ';

            _ntRendererPrintAnimationTree(animation->linked_animation);
        }
    } else if (animation->linked_animation == NULL && animation->influenced) {
        printf("\n%s ?? animation's influenced by NULL\n", __ntRendererAnimTreeText); 
    }

    if (tree_level_idx != 0) {
        __ntRendererAnimTreeText[--tree_level_idx] = 0;
    }
}

void _ntRendererResetAnimTree() {
    memset(__ntRendererAnimTreeText, 0, 64);
}