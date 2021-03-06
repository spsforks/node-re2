#include "./wrapped_re2.h"
#include "./util.h"

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

	StrVal str(info[0]);
	if (!str.data) {
		return;
	}

	size_t lastIndex = 0;

	if (str.isBuffer) {
		if ((re2->global || re2->sticky) && re2->lastIndex) {
			if (re2->lastIndex > str.size) {
				re2->lastIndex = 0;
				info.GetReturnValue().Set(false);
				return;
			}
			lastIndex = re2->lastIndex;
		}
	} else {
		if ((re2->global || re2->sticky) && re2->lastIndex) {
			if (re2->lastIndex > str.length) {
				re2->lastIndex = 0;
				info.GetReturnValue().Set(false);
				return;
			}
			for (size_t n = re2->lastIndex; n; --n) {
				lastIndex += getUtf8CharSize(str.data[lastIndex]);
			}
		}
	}

	// actual work

	if (re2->global || re2->sticky) {
		StringPiece match;
		if (re2->regexp.Match(str, lastIndex, str.size, re2->sticky ? RE2::ANCHOR_START : RE2::UNANCHORED, &match, 1)) {
			re2->lastIndex += str.isBuffer ? match.data() - str.data + match.size() - lastIndex :
				getUtf16Length(str.data + lastIndex, match.data() + match.size());
			info.GetReturnValue().Set(true);
			return;
		}
		re2->lastIndex = 0;
		info.GetReturnValue().Set(false);
		return;
	}

	info.GetReturnValue().Set(re2->regexp.Match(str, lastIndex, str.size, RE2::UNANCHORED, NULL, 0));
}
