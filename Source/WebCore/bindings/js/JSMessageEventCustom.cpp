/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2009-2018 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "JSMessageEvent.h"

#include "JSBlob.h"
#include "JSDOMBinding.h"
#include "JSDOMConvert.h"
#include "JSDOMWindow.h"
#include "JSEventTarget.h"
#include "JSMessagePort.h"
#include <JavaScriptCore/JSArray.h>
#include <JavaScriptCore/JSArrayBuffer.h>

namespace WebCore {

JSC::JSValue JSMessageEvent::ports(JSC::ExecState& state) const
{
    auto throwScope = DECLARE_THROW_SCOPE(state.vm());
    return cachedPropertyValue(state, *this, wrapped().cachedPorts(), [&] {
        JSC::JSValue ports = toJS<IDLFrozenArray<IDLInterface<MessagePort>>>(state, *globalObject(), throwScope, wrapped().ports());
        return ports;
    });
}

JSC::JSValue JSMessageEvent::data(JSC::ExecState& state) const
{
    return cachedPropertyValue(state, *this, wrapped().cachedData(), [this, &state] {
        return WTF::switchOn(wrapped().data(), [] (JSC::JSValue data) {
            return data ? data : JSC::jsNull();
        }, [this, &state] (const Ref<SerializedScriptValue>& data) {
            // FIXME: Is it best to handle errors by returning null rather than throwing an exception?
            return data->deserialize(state, globalObject(), wrapped().ports(), SerializationErrorMode::NonThrowing);
        }, [&state] (const String& data) {
            return toJS<IDLDOMString>(state, data);
        }, [this, &state] (const Ref<Blob>& data) {
            return toJS<IDLInterface<Blob>>(state, *globalObject(), data);
        }, [this, &state] (const Ref<ArrayBuffer>& data) {
            return toJS<IDLInterface<ArrayBuffer>>(state, *globalObject(), data);
        });
    });
}

void JSMessageEvent::visitAdditionalChildren(JSC::SlotVisitor& visitor)
{
    WTF::switchOn(wrapped().data(), [&visitor] (const JSValueInWrappedObject& data) {
        data.visit(visitor);
    }, [] (const Ref<SerializedScriptValue>&) {
    }, [] (const String&) {
    }, [] (const Ref<Blob>&) {
    }, [] (const Ref<ArrayBuffer>&) {
    });

    wrapped().cachedData().visit(visitor);
    wrapped().cachedPorts().visit(visitor);
}

} // namespace WebCore
