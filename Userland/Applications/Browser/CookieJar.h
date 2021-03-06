/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@pm.me>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <AK/HashMap.h>
#include <AK/Optional.h>
#include <AK/String.h>
#include <AK/Traits.h>
#include <LibCore/DateTime.h>
#include <LibWeb/Cookie/Cookie.h>
#include <LibWeb/Forward.h>

namespace Browser {

struct CookieStorageKey {
    bool operator==(const CookieStorageKey&) const = default;

    String name;
    String domain;
    String path;
};

class CookieJar {
public:
    String get_cookie(const URL& url, Web::Cookie::Source source);
    void set_cookie(const URL& url, const Web::Cookie::ParsedCookie& parsed_cookie, Web::Cookie::Source source);
    void dump_cookies() const;

private:
    static Optional<String> canonicalize_domain(const URL& url);
    static bool domain_matches(const String& string, const String& domain_string);
    static bool path_matches(const String& request_path, const String& cookie_path);
    static String default_path(const URL& url);

    void store_cookie(const Web::Cookie::ParsedCookie& parsed_cookie, const URL& url, String canonicalized_domain, Web::Cookie::Source source);
    Vector<Web::Cookie::Cookie*> get_matching_cookies(const URL& url, const String& canonicalized_domain, Web::Cookie::Source source);
    void purge_expired_cookies();

    HashMap<CookieStorageKey, Web::Cookie::Cookie> m_cookies;
};

}

namespace AK {

template<>
struct Traits<Browser::CookieStorageKey> : public GenericTraits<Browser::CookieStorageKey> {
    static unsigned hash(const Browser::CookieStorageKey& key)
    {
        unsigned hash = 0;
        hash = pair_int_hash(hash, string_hash(key.name.characters(), key.name.length()));
        hash = pair_int_hash(hash, string_hash(key.domain.characters(), key.domain.length()));
        hash = pair_int_hash(hash, string_hash(key.path.characters(), key.path.length()));
        return hash;
    }
};

}
