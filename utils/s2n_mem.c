/*
 * Copyright 2014 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "error/s2n_errno.h"

#include "utils/s2n_blob.h"
#include "utils/s2n_mem.h"

int s2n_alloc(struct s2n_blob *b, uint32_t size)
{
    b->data = NULL;
    return s2n_realloc(b, size);
}

int s2n_realloc(struct s2n_blob *b, uint32_t size)
{
    if (size == 0) {
        return s2n_free(b);
    }

    b->data = realloc(b->data, size);
    if (b->data == NULL) {
        S2N_ERROR(S2N_ERR_REALLOC);
    }
    if (mlock(b->data, size) < 0) {
        S2N_ERROR(S2N_ERR_MLOCK);
    }
#ifdef MADV_DONTDUMP
    if (madvise(b->data, size, MADV_DONTDUMP) < 0) {
        S2N_ERROR(S2N_ERR_MADVISE);
    }
#endif
    b->size = size;

    return 0;
}

int s2n_free(struct s2n_blob *b)
{
    free(b->data);
    b->data = NULL;
    b->size = 0;

    return 0;
}
