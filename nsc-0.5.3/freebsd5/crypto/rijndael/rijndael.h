/*	$KAME: rijndael.h,v 1.6 2003/08/28 08:36:32 itojun Exp $	*/
/*	$FreeBSD: src/sys/crypto/rijndael/rijndael.h,v 1.2 2003/11/11 18:58:53 ume Exp $	*/

/**
 * rijndael-alg-fst.h
 *
 * @version 3.0 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * @author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __RIJNDAEL_H
#define	__RIJNDAEL_H

#include <crypto/rijndael/rijndael-alg-fst.h>
#include <crypto/rijndael/rijndael-api-fst.h>

/* XXX: avoid conflicts with opencrypto */
#define	rijndael_set_key	_rijndael_set_key
#define	rijndael_decrypt	_rijndael_decrypt
#define	rijndael_encrypt	_rijndael_encrypt

typedef struct {
	int	decrypt;
	int	Nr;		/* key-length-dependent number of rounds */
	uint32_t ek[4 * (RIJNDAEL_MAXNR + 1)];	/* encrypt key schedule */
	uint32_t dk[4 * (RIJNDAEL_MAXNR + 1)];	/* decrypt key schedule */
} rijndael_ctx;

void	rijndael_set_key(rijndael_ctx *, const u_char *, int);
void	rijndael_decrypt(const rijndael_ctx *, const u_char *, u_char *);
void	rijndael_encrypt(const rijndael_ctx *, const u_char *, u_char *);

#endif /* __RIJNDAEL_H */
