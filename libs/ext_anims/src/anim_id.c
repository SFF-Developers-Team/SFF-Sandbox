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

#include <nt5emul/renderer_animation.h>
#include <stddef.h>

// check if specific animation id exists inside the main node
bool _ntRendererAnimIdExists(struct renderer_animation *animation, int anim_id) {
    if (animation == NULL) return false;

    // check if base node is the node we search
    if (animation->anim_id == anim_id) {
        return true;
    }

    // check if linked animation node exists
    if (animation->linked_animation != NULL) {
        // search required animation id in the linked animation
        return _ntRendererAnimIdExists(animation->linked_animation, anim_id);
    }

    return false;
}

// if not found 'assert' would be called. child nodes also are gonna be checked
double _ntRendererGetAnimationResult(struct renderer_animation *animation, int anim_id) {
    if (animation == NULL) {
        return 0;
    }

    if (animation->anim_id == anim_id) {
        return animation->current_value;
    }

    if (animation->linked_animation != NULL) {
        return _ntRendererGetAnimationResult(animation->linked_animation, anim_id);
    }

    return 0;
}