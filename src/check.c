/*
 * libpwquality main API code for quality checking
 *
 * See the end of the file for Copyright and License Information
 */
 
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <crack.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#include <curl/curl.h>
#include <openssl/sha.h>

#include "pwquality.h"


static char
hexdigit(const int digit) {
    if(digit < 10)
        return '0' + digit;
    else
        return 'a' + (digit - 10);
}

#define HASH_BUFFER_LENGTH (SHA_DIGEST_LENGTH * 2 + 1)

static void
hash_password(const char *const plaintext, char *const buffer) {
    // We put the hash digest starting at SHA_DIGEST_LENGTH
    // so we can operate on the same buffer during the conver-to-hex phase.
    SHA1(plaintext, strlen(plaintext), buffer + SHA_DIGEST_LENGTH);
    
    for(int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        buffer[i*2  ] = hexdigit(buffer[SHA_DIGEST_LENGTH + i] / 16);
        buffer[i*2+1] = hexdigit(buffer[SHA_DIGEST_LENGTH + i] % 16);
    }
    buffer[SHA_DIGEST_LENGTH*2] = '\0';
}

#define RANGE_URL            "https://api.pwnedpasswords.com/range/"
#define RANGE_URL_LENGTH     strlen(RANGE_URL)
#define RANGE_PREFIX_LENGTH  5
#deifne APIURL_BUFFER_SIZE   (RANGE_URL_LENGTH + RANGE_PREFIX_LENGTH + 1)

static void
generate_api_url(const char *const pwdhash, char *const buffer) {
    strcpy(buffer, RANGE_URL);
    const size_t len = RANGE_URL_LENGTH;

    for(int i = 0; i < RANGE_PREFIX_LENGTH; ++i) buffer[len+i] = pwdhash[i];
    buffer[len + RANGE_PREFIX_LENGTH] = '\0';
}

#define ERROR(x) { err = (x); goto on_error; }

/* Check if the password appears in the Pwned Passwords database.
 * Return value can be one of:
 *   X  > 0 : the password was pwned X times
 *   X == 0 : the password was not pwned
 *   X  < 0 : an error occurred
 */
int
pwpwned_check(const char *password) {
    int err = PWNED_ERR_SUCCESS;

    CURL *curl = curl_easy_init();
    if(curl == NULL) ERROR(PWNED_ERR_CURL);

    char hash[HASH_BUFFER_LENGTH];
    hash_password(plaintext, hash);
    char api_url[APIURL_BUFFER_SIZE];
    generate_api_url(hash, api_url)
    
    CURLcode err = curl_easy_setopt(curl, CURL_SETOPT_URL, api_url);
    if(err == CURLE_OUT_OF_MEMORY) ERROR(PWNED_ERR_NOMEM);

    err = curl_easy_setopt(curl, CURL_SETOPT_USERAGENT, PWPWNED_USER_AGENT);
    if(err == CURLE_OUT_OF_MEMORY) ERROR(PWNED_ERR_NOMEM);

on_error:
    if(curl != NULL) curl_easy_cleanup(curl);
    return err;
}

/*
 * Copyright (c) Cristian Gafton <gafton@redhat.com>, 1996.
 *                                              All rights reserved
 * Copyright (c) Red Hat, Inc, 2011, 2015
 * Copyright (c) Tomas Mraz <tm@t8m.info>, 2011, 2015
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU General Public License version 2 or later, in which case the
 * provisions of the GPL are required INSTEAD OF the above restrictions.
 *
 * THIS SOFTWARE IS PROVIDED `AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The following copyright was appended for the long password support
 * added with the libpam 0.58 release:
 *
 * Modificaton Copyright (c) Philip W. Dalrymple III <pwd@mdtsoft.com>
 *       1997. All rights reserved
 *
 * THE MODIFICATION THAT PROVIDES SUPPORT FOR LONG PASSWORD TYPE CHECKING TO
 * THIS SOFTWARE IS PROVIDED `AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
