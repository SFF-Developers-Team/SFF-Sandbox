#ifndef UTILS_H
#define UTILS_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(var, min, max) if (var < min) var = min; if (var > max) var = max;

// https://github.com/ClassiCube/ClassiCube/blob/9274d28ddba083facefa3e2598cb37ae99fb1902/src/Funcs.h#L39-L53

#define LINKEDLIST_APPEND(item, head, tail)\
if (!head) { head = item; } else { tail->next = item; }\
tail       = item;\
item->next = NULL;

#define LINKEDLIST_REMOVE(item, cur, head, tail)\
cur = head; \
if (head == item) head = item->next;\
\
while (cur) {\
	if (cur->next == item) cur->next = item->next; \
	\
	tail = cur;\
	cur  = cur->next;\
}

#define TEXTURE_RECT(tex) (Rectangle){0, 0, tex.width, tex.height}
#define REDUCE_RECT(rect, amount) {rect.x += amount; rect.y += amount; rect.width -= amount * 2.0f; rect.height -= amount * 2.0f;}

#define RECT_CENTER(rect) (Vector2){rect.x + rect.width / 2, rect.y + rect.height / 2}

#endif