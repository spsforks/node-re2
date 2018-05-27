#include "./wrapped_re2.h"

#include <vector>

#include <node_buffer.h>


using std::vector;

using v8::Local;
using v8::String;


NAN_METHOD(WrappedRE2::Test) {

	// unpack arguments

	WrappedRE2* re2 = Nan::ObjectWrap::Unwrap<WrappedRE2>(info.This());
	if (!re2) {
		info.GetReturnValue().Set(false);
		return;
	}

	vector<char> buffer;

	char*  data;
	size_t size, lastIndex = 0;
	bool   isBuffer = false;

	if (node::Buffer::HasInstance(info[0])) {
		isBuffer = true;
		size = node::Buffer::Length(info[0]);
		if ((re2->global || re2->sticky) && re2->lastIndex) {
			if (re2->lastIndex > size) {
				re2->lastIndex = 0;
				info.GetReturnValue().Set(false);
				return;
			}
			lastIndex = re2->lastIndex;
		}
		data = node::Buffer::Data(info[0]);
	} else {
		Local<String> t(info[0]->ToString());
		if ((re2->global || re2->sticky) && re2->lastIndex) {
			if (re2->lastIndex > t->Length()) {
				re2->lastIndex = 0;
				info.GetReturnValue().Set(false);
				return;
			}
		}
		buffer.resize(t->Utf8Length() + 1);
		t->WriteUtf8(&buffer[0]);
		size = buffer.size() - 1;
		data = &buffer[0];
		if ((re2->global || re2->sticky) && re2->lastIndex) {
			for (size_t n = re2->lastIndex; n; --n) {
				lastIndex += getUtf8CharSize(data[lastIndex]);
			}
		}
	}

	// actual work

	if (re2->global || re2->sticky) {
		StringPiece match;
		if (re2->regexp.Match(StringPiece(data, size), lastIndex, size, re2->sticky ? RE2::ANCHOR_START : RE2::UNANCHORED, &match, 1)) {
			re2->lastIndex += isBuffer ? match.data() - data + match.size() - lastIndex :
				getUtf16Length(data + lastIndex, match.data() + match.size());
			info.GetReturnValue().Set(true);
			return;
		}
		re2->lastIndex = 0;
		info.GetReturnValue().Set(false);
		return;
	}

	info.GetReturnValue().Set(re2->regexp.Match(StringPiece(data, size), lastIndex, size, RE2::UNANCHORED, NULL, 0));
}
