/*
 * Copyright (C) 2010-2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JSNPObject_h
#define JSNPObject_h

#if ENABLE(NETSCAPE_PLUGIN_API)

#include <JavaScriptCore/JSGlobalObject.h>
#include <JavaScriptCore/JSObject.h>
#include <JavaScriptCore/ObjectPrototype.h>

typedef void* NPIdentifier;
struct NPObject;

namespace WebKit {

class NPRuntimeObjectMap;
    
// JSNPObject is a JSObject that wraps an NPObject.

class JSNPObject final : public JSC::JSDestructibleObject {
public:
    typedef JSC::JSDestructibleObject Base;
    static constexpr unsigned StructureFlags = Base::StructureFlags | JSC::OverridesGetOwnPropertySlot | JSC::OverridesGetPropertyNames | JSC::OverridesGetCallData;

    template<typename CellType, JSC::SubspaceAccess>
    static JSC::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        return subspaceForImpl(vm);
    }

    static JSNPObject* create(JSC::JSGlobalObject* globalObject, NPRuntimeObjectMap* objectMap, NPObject* npObject)
    {
        JSC::Structure* structure = createStructure(globalObject->vm(), globalObject, globalObject->objectPrototype());
        JSNPObject* object = new (JSC::allocateCell<JSNPObject>(globalObject->vm().heap)) JSNPObject(globalObject, structure, objectMap, npObject);
        object->finishCreation(globalObject);
        return object;
    }

    ~JSNPObject();
    static void destroy(JSCell*);

    void invalidate();

    // Used to invalidate an NPObject asynchronously.
    NPObject* leakNPObject();

    JSC::JSValue callMethod(JSC::ExecState*, NPIdentifier methodName);
    JSC::JSValue callObject(JSC::ExecState*);
    JSC::JSValue callConstructor(JSC::ExecState*);

    DECLARE_INFO;

    NPObject* npObject() const { return m_npObject; }

protected:
    void finishCreation(JSC::JSGlobalObject*);

private:
    static JSC::IsoSubspace* subspaceForImpl(JSC::VM&);
    
    JSNPObject(JSC::JSGlobalObject*, JSC::Structure*, NPRuntimeObjectMap*, NPObject*);
    
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::CallType getCallData(JSC::JSCell*, JSC::CallData&);
    static JSC::ConstructType getConstructData(JSC::JSCell*, JSC::ConstructData&);

    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static bool put(JSC::JSCell*, JSC::ExecState*, JSC::PropertyName, JSC::JSValue, JSC::PutPropertySlot&);

    static bool deleteProperty(JSC::JSCell*, JSC::ExecState*, JSC::PropertyName);
    static bool deletePropertyByIndex(JSC::JSCell*, JSC::ExecState*, unsigned propertyName);

    bool deleteProperty(JSC::ExecState*, NPIdentifier propertyName);

    static void getOwnPropertyNames(JSC::JSObject*, JSC::ExecState*, JSC::PropertyNameArray&, JSC::EnumerationMode);

    static JSC::EncodedJSValue propertyGetter(JSC::ExecState*, JSC::EncodedJSValue, JSC::PropertyName);
    static JSC::EncodedJSValue methodGetter(JSC::ExecState*, JSC::EncodedJSValue, JSC::PropertyName);
    static JSC::Exception* throwInvalidAccessError(JSC::ExecState*, JSC::ThrowScope&);

    NPRuntimeObjectMap* m_objectMap;
    NPObject* m_npObject;
};

} // namespace WebKit

#endif // ENABLE(NETSCAPE_PLUGIN_API)

#endif // JSNPObject_h
