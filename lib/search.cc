#include "./wrapped_re2.h"
#include "./util.h"


using v8::Integer;


NAN_METHOD(WrappedRE2::Search) {

	// unpack arguments

	WrappedRE2* re2 = Nan::ObjectWrap::Unwrap<WrappedRE2>(info.This());
	if (!re2) {
		info.GetReturnValue().Set(-1);
		return;
	}

	StrVal a(info[0]);
	if (!a.data) {
		return;
	}

	// actual work

	StringPiece match;

	if (re2->regexp.Match(a, 0, a.size, re2->sticky ? RE2::ANCHOR_START : RE2::UNANCHORED, &match, 1)) {
		info.GetReturnValue().Set(static_cast<int>(a.isBuffer ? match.data() - a.data :
			getUtf16Length(a.data, match.data())));
		return;
	}

	info.GetReturnValue().Set(-1);
}
