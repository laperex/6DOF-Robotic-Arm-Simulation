#include "UndoStack.h"

struct Stack {
	util::undo::Data* array = nullptr;

	int pop_idx = 0, push_idx = 0, size = 0;
};

static Stack stack;

#define self (&stack)

void util::undo::Initialize(int size) {
	self->size = size;
	self->array = new Data[size];
}

void util::undo::Free() {
	self->size = 0;
	self->pop_idx = 0;
	self->push_idx = 0;
	delete[] self->array;
	self->array = nullptr;
}

