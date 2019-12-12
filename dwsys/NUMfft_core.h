/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggSQUISH SOFTWARE CODEC SOURCE CODE.   *
 *                                                                  *
 ********************************************************************

  function: Fast discrete Fourier and cosine transforms and inverses
  author: Monty <xiphmont@mit.edu>
  modifications by: Monty
  last modification date: Jul 1 1996
  
  djmw 20030630 Adapted for praat (replaced 'int' declarations with 'long').
  djmw 20040511 Made all local variables type double to increase numerical precision.
  djmw 20171003 Replaced `long` declarations with `integer`).

 ********************************************************************/

/* These Fourier routines were originally based on the Fourier routines of
   the same names from the NETLIB bihar and fftpack fortran libraries
   developed by Paul N. Swarztrauber at the National Center for Atmospheric
   Research in Boulder, CO USA.  They have been reimplemented in C and
   optimized in a few ways for OggSquish. */

/* As the original fortran libraries are public domain, the C Fourier
   routines in this file are hereby released to the public domain as well.
   The C routines here produce output exactly equivalent to the original
   fortran routines.  Of particular interest are the facts that (like the
   original fortran), these routines can work on arbitrary length vectors
   that need not be powers of two in length. */

#include "melder.h"   /* for integer */

static void drfti1 (integer n, FFT_DATA_TYPE * wa, integer *ifac)
{
	static constexpr integer ntryh[4] = { 4, 2, 3, 5 };
	static constexpr double tpi = 6.28318530717958647692528676655900577;
	integer ntry = 0, j = -1;
	integer nl = n;
	integer nf = 0;

  L101:
	j++;
	if (j < 4)
		ntry = ntryh[j];
	else
		ntry += 2;

  L104:
	const integer nq = nl / ntry;
	const integer nr = nl - ntry * nq;
	if (nr != 0)
		goto L101;

	nf++;
	ifac[nf + 1] = ntry;
	nl = nq;
	if (ntry != 2)
		goto L107;
	if (nf == 1)
		goto L107;

	for (integer i = 1; i < nf; i++)
	{
		const integer ib = nf - i + 1;
		ifac[ib + 1] = ifac[ib];
	}
	ifac[2] = 2;

  L107:
	if (nl != 1)
		goto L104;
	ifac[0] = n;
	ifac[1] = nf;
	const double argh = tpi / n;
	integer is = 0;
	const integer nfm1 = nf - 1;
	integer l1 = 1;

	if (nfm1 == 0)
		return;
	for (integer k1 = 0; k1 < nfm1; k1++)
	{
		const integer ip = ifac[k1 + 2];
		integer ld = 0;
		const integer l2 = l1 * ip;
		const integer ido = n / l2;
		const integer ipm = ip - 1;

		for (j = 0; j < ipm; j++)
		{
			ld += l1;
			integer i = is;
			const double argld = (double) ld *argh;

			double fi = 0.0;
			for (integer ii = 2; ii < ido; ii += 2)
			{
				fi += 1.0;
				const double arg = fi * argld;
				wa[i++] = cos (arg);
				wa[i++] = sin (arg);
			}
			is += ido;
		}
		l1 = l2;
	}
}

static void NUMrffti (integer n, FFT_DATA_TYPE * wsave, integer *ifac)
{
	if (n == 1)
		return;
	drfti1 (n, wsave + n, ifac);
}

/* void NUMcosqi(integer n, FFT_DATA_TYPE *wsave, integer *ifac){ static
   double pih = 1.57079632679489661923132169163975; static integer k;
   static double fk, dt;

   dt=pih/n; fk=0.; for(k=0;k<n;k++){ fk+=1.; wsave[k] = cos(fk*dt); }

   NUMrffti(n, wsave+n,ifac); } */

static void dradf2 (integer ido, integer l1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa1)
{
	integer t1 = 0;
	integer t2, t0 = (t2 = l1 * ido);
	integer t3 = ido << 1;
	for (integer k = 0; k < l1; k++)
	{
		ch[t1 << 1] = cc[t1] + cc[t2];
		ch[(t1 << 1) + t3 - 1] = cc[t1] - cc[t2];
		t1 += ido;
		t2 += ido;
	}

	if (ido < 2)
		return;
	if (ido == 2)
		goto L105;

	t1 = 0;
	t2 = t0;
	for (integer k = 0; k < l1; k++)
	{
		t3 = t2;
		integer t4 = (t1 << 1) + (ido << 1);
		integer t5 = t1;
		integer t6 = t1 + t1;
		for (integer i = 2; i < ido; i += 2)
		{
			t3 += 2;
			t4 -= 2;
			t5 += 2;
			t6 += 2;
			const double tr2 = wa1[i - 2] * cc[t3 - 1] + wa1[i - 1] * cc[t3];
			const double ti2 = wa1[i - 2] * cc[t3] - wa1[i - 1] * cc[t3 - 1];
			ch[t6] = cc[t5] + ti2;
			ch[t4] = ti2 - cc[t5];
			ch[t6 - 1] = cc[t5 - 1] + tr2;
			ch[t4 - 1] = cc[t5 - 1] - tr2;
		}
		t1 += ido;
		t2 += ido;
	}

	if (ido % 2 == 1)
		return;

  L105:
	t3 = (t2 = (t1 = ido) - 1);
	t2 += t0;
	for (integer k = 0; k < l1; k++)
	{
		ch[t1] = -cc[t2];
		ch[t1 - 1] = cc[t3];
		t1 += ido << 1;
		t2 += ido;
		t3 += ido;
	}
}

static void dradf4 (integer ido, integer l1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa1,
	FFT_DATA_TYPE * wa2, FFT_DATA_TYPE * wa3)
{
	static constexpr double hsqt2 = .70710678118654752440084436210485;
	integer t5, t6;

	integer t0 = l1 * ido;

	integer t1 = t0;
	integer t4 = t1 << 1;
	integer t2 = t1 + (t1 << 1);
	integer t3 = 0;

	for (integer k = 0; k < l1; k++)
	{
		const double tr1 = cc[t1] + cc[t2];
		const double tr2 = cc[t3] + cc[t4];
		ch[t5 = t3 << 2] = tr1 + tr2;
		ch[(ido << 2) + t5 - 1] = tr2 - tr1;
		ch[(t5 += (ido << 1)) - 1] = cc[t3] - cc[t4];
		ch[t5] = cc[t2] - cc[t1];

		t1 += ido;
		t2 += ido;
		t3 += ido;
		t4 += ido;
	}

	if (ido < 2)
		return;
	if (ido == 2)
		goto L105;

	t1 = 0;
	for (integer k = 0; k < l1; k++)
	{
		t2 = t1;
		t4 = t1 << 2;
		t5 = (t6 = ido << 1) + t4;
		for (integer i = 2; i < ido; i += 2)
		{
			t3 = (t2 += 2);
			t4 += 2;
			t5 -= 2;

			t3 += t0;
			const double cr2 = wa1[i - 2] * cc[t3 - 1] + wa1[i - 1] * cc[t3];
			const double ci2 = wa1[i - 2] * cc[t3] - wa1[i - 1] * cc[t3 - 1];
			t3 += t0;
			const double cr3 = wa2[i - 2] * cc[t3 - 1] + wa2[i - 1] * cc[t3];
			const double ci3 = wa2[i - 2] * cc[t3] - wa2[i - 1] * cc[t3 - 1];
			t3 += t0;
			const double cr4 = wa3[i - 2] * cc[t3 - 1] + wa3[i - 1] * cc[t3];
			const double ci4 = wa3[i - 2] * cc[t3] - wa3[i - 1] * cc[t3 - 1];

			const double tr1 = cr2 + cr4;
			const double tr4 = cr4 - cr2;
			const double ti1 = ci2 + ci4;
			const double ti4 = ci2 - ci4;
			const double ti2 = cc[t2] + ci3;
			const double ti3 = cc[t2] - ci3;
			const double tr2 = cc[t2 - 1] + cr3;
			const double tr3 = cc[t2 - 1] - cr3;

			ch[t4 - 1] = tr1 + tr2;
			ch[t4] = ti1 + ti2;

			ch[t5 - 1] = tr3 - ti4;
			ch[t5] = tr4 - ti3;

			ch[t4 + t6 - 1] = ti4 + tr3;
			ch[t4 + t6] = tr4 + ti3;

			ch[t5 + t6 - 1] = tr2 - tr1;
			ch[t5 + t6] = ti1 - ti2;
		}
		t1 += ido;
	}
	if (ido % 2 == 1)
		return;

  L105:

	t2 = (t1 = t0 + ido - 1) + (t0 << 1);
	t3 = ido << 2;
	t4 = ido;
	t5 = ido << 1;
	t6 = ido;

	for (integer k = 0; k < l1; k++)
	{
		const double ti1 = -hsqt2 * (cc[t1] + cc[t2]);
		const double tr1 = hsqt2 * (cc[t1] - cc[t2]);
		ch[t4 - 1] = tr1 + cc[t6 - 1];
		ch[t4 + t5 - 1] = cc[t6 - 1] - tr1;
		ch[t4] = ti1 - cc[t1 + t0];
		ch[t4 + t5] = ti1 + cc[t1 + t0];
		t1 += ido;
		t2 += ido;
		t4 += t3;
		t6 += ido;
	}
}

static void dradfg (integer ido, integer ip, integer l1, integer idl1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * c1,
	FFT_DATA_TYPE * c2, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * ch2, FFT_DATA_TYPE * wa)
{

	static constexpr double tpi = 6.28318530717958647692528676655900577;
	integer is;
	integer t1, t2, t3, t4, t5, t6, t7, t8, t9;

	const double arg = tpi / (double) ip;
	const double dcp = cos (arg);
	const double dsp = sin (arg);
	const integer ipph = (ip + 1) >> 1;
	const integer ipp2 = ip;
	const integer idp2 = ido;
	const integer nbd = (ido - 1) >> 1;
	const integer t0 = l1 * ido;
	const integer t10 = ip * ido;

	if (ido == 1)
		goto L119;
	for (integer ik = 0; ik < idl1; ik++)
		ch2[ik] = c2[ik];

	t1 = 0;
	for (integer j = 1; j < ip; j++)
	{
		t1 += t0;
		t2 = t1;
		for (integer k = 0; k < l1; k++)
		{
			ch[t2] = c1[t2];
			t2 += ido;
		}
	}

	is = -ido;
	t1 = 0;
	if (nbd > l1)
	{
		for (integer j = 1; j < ip; j++)
		{
			t1 += t0;
			is += ido;
			t2 = -ido + t1;
			for (integer k = 0; k < l1; k++)
			{
				integer idij = is - 1;
				t2 += ido;
				t3 = t2;
				for (integer i = 2; i < ido; i += 2)
				{
					idij += 2;
					t3 += 2;
					ch[t3 - 1] = wa[idij - 1] * c1[t3 - 1] + wa[idij] * c1[t3];
					ch[t3] = wa[idij - 1] * c1[t3] - wa[idij] * c1[t3 - 1];
				}
			}
		}
	}
	else
	{

		for (integer j = 1; j < ip; j++)
		{
			is += ido;
			integer idij = is - 1;
			t1 += t0;
			t2 = t1;
			for (integer i = 2; i < ido; i += 2)
			{
				idij += 2;
				t2 += 2;
				t3 = t2;
				for (integer k = 0; k < l1; k++)
				{
					ch[t3 - 1] = wa[idij - 1] * c1[t3 - 1] + wa[idij] * c1[t3];
					ch[t3] = wa[idij - 1] * c1[t3] - wa[idij] * c1[t3 - 1];
					t3 += ido;
				}
			}
		}
	}

	t1 = 0;
	t2 = ipp2 * t0;
	if (nbd < l1)
	{
		for (integer j = 1; j < ipph; j++)
		{
			t1 += t0;
			t2 -= t0;
			t3 = t1;
			t4 = t2;
			for (integer i = 2; i < ido; i += 2)
			{
				t3 += 2;
				t4 += 2;
				t5 = t3 - ido;
				t6 = t4 - ido;
				for (integer k = 0; k < l1; k++)
				{
					t5 += ido;
					t6 += ido;
					c1[t5 - 1] = ch[t5 - 1] + ch[t6 - 1];
					c1[t6 - 1] = ch[t5] - ch[t6];
					c1[t5] = ch[t5] + ch[t6];
					c1[t6] = ch[t6 - 1] - ch[t5 - 1];
				}
			}
		}
	}
	else
	{
		for (integer j = 1; j < ipph; j++)
		{
			t1 += t0;
			t2 -= t0;
			t3 = t1;
			t4 = t2;
			for (integer k = 0; k < l1; k++)
			{
				t5 = t3;
				t6 = t4;
				for (integer i = 2; i < ido; i += 2)
				{
					t5 += 2;
					t6 += 2;
					c1[t5 - 1] = ch[t5 - 1] + ch[t6 - 1];
					c1[t6 - 1] = ch[t5] - ch[t6];
					c1[t5] = ch[t5] + ch[t6];
					c1[t6] = ch[t6 - 1] - ch[t5 - 1];
				}
				t3 += ido;
				t4 += ido;
			}
		}
	}

  L119:
	for (integer ik = 0; ik < idl1; ik++)
		c2[ik] = ch2[ik];

	t1 = 0;
	t2 = ipp2 * idl1;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1 - ido;
		t4 = t2 - ido;
		for (integer k = 0; k < l1; k++)
		{
			t3 += ido;
			t4 += ido;
			c1[t3] = ch[t3] + ch[t4];
			c1[t4] = ch[t4] - ch[t3];
		}
	}

	double ar1 = 1.;
	double ai1 = 0.;
	t1 = 0;
	t2 = ipp2 * idl1;
	t3 = (ip - 1) * idl1;
	for (integer l = 1; l < ipph; l++)
	{
		t1 += idl1;
		t2 -= idl1;
		const double ar1h = dcp * ar1 - dsp * ai1;
		ai1 = dcp * ai1 + dsp * ar1;
		ar1 = ar1h;
		t4 = t1;
		t5 = t2;
		t6 = t3;
		t7 = idl1;

		for (integer ik = 0; ik < idl1; ik++)
		{
			ch2[t4++] = c2[ik] + ar1 * c2[t7++];
			ch2[t5++] = ai1 * c2[t6++];
		}

		const double dc2 = ar1;
		const double ds2 = ai1;
		double ar2 = ar1;
		double ai2 = ai1;

		t4 = idl1;
		t5 = (ipp2 - 1) * idl1;
		for (integer j = 2; j < ipph; j++)
		{
			t4 += idl1;
			t5 -= idl1;

			const double ar2h = dc2 * ar2 - ds2 * ai2;
			ai2 = dc2 * ai2 + ds2 * ar2;
			ar2 = ar2h;

			t6 = t1;
			t7 = t2;
			t8 = t4;
			t9 = t5;
			for (integer ik = 0; ik < idl1; ik++)
			{
				ch2[t6++] += ar2 * c2[t8++];
				ch2[t7++] += ai2 * c2[t9++];
			}
		}
	}

	t1 = 0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += idl1;
		t2 = t1;
		for (integer ik = 0; ik < idl1; ik++)
			ch2[ik] += c2[t2++];
	}

	if (ido < l1)
		goto L132;

	t1 = 0;
	t2 = 0;
	for (integer k = 0; k < l1; k++)
	{
		t3 = t1;
		t4 = t2;
		for (integer i = 0; i < ido; i++)
			cc[t4++] = ch[t3++];
		t1 += ido;
		t2 += t10;
	}

	goto L135;

  L132:
	for (integer i = 0; i < ido; i++)
	{
		t1 = i;
		t2 = i;
		for (integer k = 0; k < l1; k++)
		{
			cc[t2] = ch[t1];
			t1 += ido;
			t2 += t10;
		}
	}

  L135:
	t1 = 0;
	t2 = ido << 1;
	t3 = 0;
	t4 = ipp2 * t0;
	for (integer j = 1; j < ipph; j++)
	{

		t1 += t2;
		t3 += t0;
		t4 -= t0;

		t5 = t1;
		t6 = t3;
		t7 = t4;

		for (integer k = 0; k < l1; k++)
		{
			cc[t5 - 1] = ch[t6];
			cc[t5] = ch[t7];
			t5 += t10;
			t6 += ido;
			t7 += ido;
		}
	}

	if (ido == 1)
		return;
	if (nbd < l1)
		goto L141;

	t1 = -ido;
	t3 = 0;
	t4 = 0;
	t5 = ipp2 * t0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t2;
		t3 += t2;
		t4 += t0;
		t5 -= t0;
		t6 = t1;
		t7 = t3;
		t8 = t4;
		t9 = t5;
		for (integer k = 0; k < l1; k++)
		{
			for (integer i = 2; i < ido; i += 2)
			{
				const integer ic = idp2 - i;
				cc[i + t7 - 1] = ch[i + t8 - 1] + ch[i + t9 - 1];
				cc[ic + t6 - 1] = ch[i + t8 - 1] - ch[i + t9 - 1];
				cc[i + t7] = ch[i + t8] + ch[i + t9];
				cc[ic + t6] = ch[i + t9] - ch[i + t8];
			}
			t6 += t10;
			t7 += t10;
			t8 += ido;
			t9 += ido;
		}
	}
	return;

  L141:

	t1 = -ido;
	t3 = 0;
	t4 = 0;
	t5 = ipp2 * t0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t2;
		t3 += t2;
		t4 += t0;
		t5 -= t0;
		for (integer i = 2; i < ido; i += 2)
		{
			t6 = idp2 + t1 - i;
			t7 = i + t3;
			t8 = i + t4;
			t9 = i + t5;
			for (integer k = 0; k < l1; k++)
			{
				cc[t7 - 1] = ch[t8 - 1] + ch[t9 - 1];
				cc[t6 - 1] = ch[t8 - 1] - ch[t9 - 1];
				cc[t7] = ch[t8] + ch[t9];
				cc[t6] = ch[t9] - ch[t8];
				t6 += t10;
				t7 += t10;
				t8 += ido;
				t9 += ido;
			}
		}
	}
}

static void drftf1 (integer n, FFT_DATA_TYPE * c, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa, integer *ifac)
{
	const integer nf = ifac[1];
	integer na = 1;
	integer l2 = n;
	integer iw = n;

	for (integer k1 = 0; k1 < nf; k1++)
	{
		const integer kh = nf - k1;
		const integer ip = ifac[kh + 1];
		const integer l1 = l2 / ip;
		const integer ido = n / l2;
		const integer idl1 = ido * l1;
		integer ix2, ix3;
		iw -= (ip - 1) * ido;
		na = 1 - na;

		if (ip != 4)
			goto L102;

		ix2 = iw + ido;
		ix3 = ix2 + ido;
		if (na != 0)
			dradf4 (ido, l1, ch, c, wa + iw - 1, wa + ix2 - 1, wa + ix3 - 1);
		else
			dradf4 (ido, l1, c, ch, wa + iw - 1, wa + ix2 - 1, wa + ix3 - 1);
		goto L110;

	  L102:
		if (ip != 2)
			goto L104;
		if (na != 0)
			goto L103;

		dradf2 (ido, l1, c, ch, wa + iw - 1);
		goto L110;

	  L103:
		dradf2 (ido, l1, ch, c, wa + iw - 1);
		goto L110;

	  L104:
		if (ido == 1)
			na = 1 - na;
		if (na != 0)
			goto L109;

		dradfg (ido, ip, l1, idl1, c, c, c, ch, ch, wa + iw - 1);
		na = 1;
		goto L110;

	  L109:
		dradfg (ido, ip, l1, idl1, ch, ch, ch, c, c, wa + iw - 1);
		na = 0;

	  L110:
		l2 = l1;
	}

	if (na == 1)
		return;

	for (integer i = 0; i < n; i++)
		c[i] = ch[i];
}

static void dradb2 (integer ido, integer l1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa1)
{
	const integer t0 = l1 * ido;

	integer t1 = 0;
	integer t2 = 0;
	integer t3 = (ido << 1) - 1;
	for (integer k = 0; k < l1; k++)
	{
		ch[t1] = cc[t2] + cc[t3 + t2];
		ch[t1 + t0] = cc[t2] - cc[t3 + t2];
		t2 = (t1 += ido) << 1;
	}

	if (ido < 2)
		return;
	if (ido == 2)
		goto L105;

	t1 = 0;
	t2 = 0;
	for (integer k = 0; k < l1; k++)
	{
		t3 = t1;
		integer t4, t5 = (t4 = t2) + (ido << 1);
		integer t6 = t0 + t1;
		for (integer i = 2; i < ido; i += 2)
		{
			t3 += 2;
			t4 += 2;
			t5 -= 2;
			t6 += 2;
			ch[t3 - 1] = cc[t4 - 1] + cc[t5 - 1];
			const double tr2 = cc[t4 - 1] - cc[t5 - 1];
			ch[t3] = cc[t4] - cc[t5];
			const double ti2 = cc[t4] + cc[t5];
			ch[t6 - 1] = wa1[i - 2] * tr2 - wa1[i - 1] * ti2;
			ch[t6] = wa1[i - 2] * ti2 + wa1[i - 1] * tr2;
		}
		t2 = (t1 += ido) << 1;
	}

	if (ido % 2 == 1)
		return;

  L105:
	t1 = ido - 1;
	t2 = ido - 1;
	for (integer k = 0; k < l1; k++)
	{
		ch[t1] = cc[t2] + cc[t2];
		ch[t1 + t0] = -(cc[t2 + 1] + cc[t2 + 1]);
		t1 += ido;
		t2 += ido << 1;
	}
}

static void dradb3 (integer ido, integer l1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa1,
	FFT_DATA_TYPE * wa2)
{
	static constexpr double taur = -.5;
	static constexpr double taui = .86602540378443864676372317075293618;

	integer t0 = l1 * ido;

	integer t1 = 0;
	integer t2 = t0 << 1;
	integer t3 = ido << 1;
	integer t4 = ido + (ido << 1);
	integer t5 = 0;
	for (integer k = 0; k < l1; k++)
	{
		const double tr2 = cc[t3 - 1] + cc[t3 - 1];
		const double cr2 = cc[t5] + (taur * tr2);
		ch[t1] = cc[t5] + tr2;
		const double ci3 = taui * (cc[t3] + cc[t3]);
		ch[t1 + t0] = cr2 - ci3;
		ch[t1 + t2] = cr2 + ci3;
		t1 += ido;
		t3 += t4;
		t5 += t4;
	}

	if (ido == 1)
		return;

	t1 = 0;
	t3 = ido << 1;
	for (integer k = 0; k < l1; k++)
	{
		integer t7 = t1 + (t1 << 1);
		integer t6 = (t5 = t7 + t3);
		integer t8 = t1;
		integer t9, t10 = (t9 = t1 + t0) + t0;

		for (integer i = 2; i < ido; i += 2)
		{
			t5 += 2;
			t6 -= 2;
			t7 += 2;
			t8 += 2;
			t9 += 2;
			t10 += 2;
			const double tr2 = cc[t5 - 1] + cc[t6 - 1];
			const double cr2 = cc[t7 - 1] + (taur * tr2);
			ch[t8 - 1] = cc[t7 - 1] + tr2;
			const double ti2 = cc[t5] - cc[t6];
			const double ci2 = cc[t7] + (taur * ti2);
			ch[t8] = cc[t7] + ti2;
			const double cr3 = taui * (cc[t5 - 1] - cc[t6 - 1]);
			const double ci3 = taui * (cc[t5] + cc[t6]);
			const double dr2 = cr2 - ci3;
			const double dr3 = cr2 + ci3;
			const double di2 = ci2 + cr3;
			const double di3 = ci2 - cr3;
			ch[t9 - 1] = wa1[i - 2] * dr2 - wa1[i - 1] * di2;
			ch[t9] = wa1[i - 2] * di2 + wa1[i - 1] * dr2;
			ch[t10 - 1] = wa2[i - 2] * dr3 - wa2[i - 1] * di3;
			ch[t10] = wa2[i - 2] * di3 + wa2[i - 1] * dr3;
		}
		t1 += ido;
	}
}

static void dradb4 (integer ido, integer l1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa1,
	FFT_DATA_TYPE * wa2, FFT_DATA_TYPE * wa3)
{
	static constexpr double sqrt2 = 1.4142135623730950488016887242097;

	const integer t0 = l1 * ido;

	integer t1 = 0;
	integer t2 = ido << 2;
	integer t3 = 0, t4, t5, t8;
	const integer t6 = ido << 1;
	for (integer k = 0; k < l1; k++)
	{
		t4 = t3 + t6;
		t5 = t1;
		const double tr3 = cc[t4 - 1] + cc[t4 - 1];
		const double tr4 = cc[t4] + cc[t4];
		const double tr1 = cc[t3] - cc[(t4 += t6) - 1];
		const double tr2 = cc[t3] + cc[t4 - 1];
		ch[t5] = tr2 + tr3;
		ch[t5 += t0] = tr1 - tr4;
		ch[t5 += t0] = tr2 - tr3;
		ch[t5 += t0] = tr1 + tr4;
		t1 += ido;
		t3 += t2;
	}

	if (ido < 2)
		return;
	if (ido == 2)
		goto L105;

	t1 = 0;
	for (integer k = 0; k < l1; k++)
	{
		integer t5 = (t4 = (t3 = (t2 = t1 << 2) + t6)) + t6;
		integer t7 = t1;
		for (integer i = 2; i < ido; i += 2)
		{
			t2 += 2;
			t3 += 2;
			t4 -= 2;
			t5 -= 2;
			t7 += 2;
			const double ti1 = cc[t2] + cc[t5];
			const double ti2 = cc[t2] - cc[t5];
			const double ti3 = cc[t3] - cc[t4];
			const double tr4 = cc[t3] + cc[t4];
			const double tr1 = cc[t2 - 1] - cc[t5 - 1];
			const double tr2 = cc[t2 - 1] + cc[t5 - 1];
			const double ti4 = cc[t3 - 1] - cc[t4 - 1];
			const double tr3 = cc[t3 - 1] + cc[t4 - 1];
			ch[t7 - 1] = tr2 + tr3;
			const double cr3 = tr2 - tr3;
			ch[t7] = ti2 + ti3;
			const double ci3 = ti2 - ti3;
			const double cr2 = tr1 - tr4;
			const double cr4 = tr1 + tr4;
			const double ci2 = ti1 + ti4;
			const double ci4 = ti1 - ti4;

			ch[(t8 = t7 + t0) - 1] = wa1[i - 2] * cr2 - wa1[i - 1] * ci2;
			ch[t8] = wa1[i - 2] * ci2 + wa1[i - 1] * cr2;
			ch[(t8 += t0) - 1] = wa2[i - 2] * cr3 - wa2[i - 1] * ci3;
			ch[t8] = wa2[i - 2] * ci3 + wa2[i - 1] * cr3;
			ch[(t8 += t0) - 1] = wa3[i - 2] * cr4 - wa3[i - 1] * ci4;
			ch[t8] = wa3[i - 2] * ci4 + wa3[i - 1] * cr4;
		}
		t1 += ido;
	}

	if (ido % 2 == 1)
		return;

  L105:

	t1 = ido;
	t2 = ido << 2;
	t3 = ido - 1;
	t4 = ido + (ido << 1);
	for (integer k = 0; k < l1; k++)
	{
		t5 = t3;
		const double ti1 = cc[t1] + cc[t4];
		const double ti2 = cc[t4] - cc[t1];
		const double tr1 = cc[t1 - 1] - cc[t4 - 1];
		const double tr2 = cc[t1 - 1] + cc[t4 - 1];
		ch[t5] = tr2 + tr2;
		ch[t5 += t0] = sqrt2 * (tr1 - ti1);
		ch[t5 += t0] = ti2 + ti2;
		ch[t5 += t0] = -sqrt2 * (tr1 + ti1);

		t3 += ido;
		t1 += t2;
		t4 += t2;
	}
}

static void dradbg (integer ido, integer ip, integer l1, integer idl1, FFT_DATA_TYPE * cc, FFT_DATA_TYPE * c1,
	FFT_DATA_TYPE * c2, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * ch2, FFT_DATA_TYPE * wa)
{
	static constexpr double tpi = 6.28318530717958647692528676655900577;
	integer is, t1, t2, t3, t4, t5, t6, t7, t8, t9, t11, t12;

	const integer t10 = ip * ido;
	const integer t0 = l1 * ido;
	const double arg = tpi / (double) ip;
	const double dcp = cos (arg);
	const double dsp = sin (arg);
	const integer nbd = (ido - 1) >> 1;
	const integer ipp2 = ip;
	const integer ipph = (ip + 1) >> 1;
	if (ido < l1)
		goto L103;

	t1 = 0;
	t2 = 0;
	for (integer k = 0; k < l1; k++)
	{
		t3 = t1;
		t4 = t2;
		for (integer i = 0; i < ido; i++)
		{
			ch[t3] = cc[t4];
			t3++;
			t4++;
		}
		t1 += ido;
		t2 += t10;
	}
	goto L106;

  L103:
	t1 = 0;
	for (integer i = 0; i < ido; i++)
	{
		t2 = t1;
		t3 = t1;
		for (integer k = 0; k < l1; k++)
		{
			ch[t2] = cc[t3];
			t2 += ido;
			t3 += t10;
		}
		t1++;
	}

  L106:
	t1 = 0;
	t2 = ipp2 * t0;
	t7 = (t5 = ido << 1);
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1;
		t4 = t2;
		t6 = t5;
		for (integer k = 0; k < l1; k++)
		{
			ch[t3] = cc[t6 - 1] + cc[t6 - 1];
			ch[t4] = cc[t6] + cc[t6];
			t3 += ido;
			t4 += ido;
			t6 += t10;
		}
		t5 += t7;
	}

	if (ido == 1)
		goto L116;
	if (nbd < l1)
		goto L112;

	t1 = 0;
	t2 = ipp2 * t0;
	t7 = 0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1;
		t4 = t2;

		t7 += (ido << 1);
		t8 = t7;
		for (integer k = 0; k < l1; k++)
		{
			t5 = t3;
			t6 = t4;
			t9 = t8;
			t11 = t8;
			for (integer i = 2; i < ido; i += 2)
			{
				t5 += 2;
				t6 += 2;
				t9 += 2;
				t11 -= 2;
				ch[t5 - 1] = cc[t9 - 1] + cc[t11 - 1];
				ch[t6 - 1] = cc[t9 - 1] - cc[t11 - 1];
				ch[t5] = cc[t9] - cc[t11];
				ch[t6] = cc[t9] + cc[t11];
			}
			t3 += ido;
			t4 += ido;
			t8 += t10;
		}
	}
	goto L116;

  L112:
	t1 = 0;
	t2 = ipp2 * t0;
	t7 = 0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1;
		t4 = t2;
		t7 += (ido << 1);
		t8 = t7;
		t9 = t7;
		for (integer i = 2; i < ido; i += 2)
		{
			t3 += 2;
			t4 += 2;
			t8 += 2;
			t9 -= 2;
			t5 = t3;
			t6 = t4;
			t11 = t8;
			t12 = t9;
			for (integer k = 0; k < l1; k++)
			{
				ch[t5 - 1] = cc[t11 - 1] + cc[t12 - 1];
				ch[t6 - 1] = cc[t11 - 1] - cc[t12 - 1];
				ch[t5] = cc[t11] - cc[t12];
				ch[t6] = cc[t11] + cc[t12];
				t5 += ido;
				t6 += ido;
				t11 += t10;
				t12 += t10;
			}
		}
	}

  L116:
	double ar1 = 1.;
	double ai1 = 0.;
	t1 = 0;
	t9 = (t2 = ipp2 * idl1);
	t3 = (ip - 1) * idl1;
	for (integer l = 1; l < ipph; l++)
	{
		t1 += idl1;
		t2 -= idl1;

		const double ar1h = dcp * ar1 - dsp * ai1;
		ai1 = dcp * ai1 + dsp * ar1;
		ar1 = ar1h;
		t4 = t1;
		t5 = t2;
		t6 = 0;
		t7 = idl1;
		t8 = t3;
		for (integer ik = 0; ik < idl1; ik++)
		{
			c2[t4++] = ch2[t6++] + ar1 * ch2[t7++];
			c2[t5++] = ai1 * ch2[t8++];
		}
		const double dc2 = ar1;
		const double ds2 = ai1;
		double ar2 = ar1;
		double ai2 = ai1;

		t6 = idl1;
		t7 = t9 - idl1;
		for (integer j = 2; j < ipph; j++)
		{
			t6 += idl1;
			t7 -= idl1;
			const double ar2h = dc2 * ar2 - ds2 * ai2;
			ai2 = dc2 * ai2 + ds2 * ar2;
			ar2 = ar2h;
			t4 = t1;
			t5 = t2;
			t11 = t6;
			t12 = t7;
			for (integer ik = 0; ik < idl1; ik++)
			{
				c2[t4++] += ar2 * ch2[t11++];
				c2[t5++] += ai2 * ch2[t12++];
			}
		}
	}

	t1 = 0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += idl1;
		t2 = t1;
		for (integer ik = 0; ik < idl1; ik++)
			ch2[ik] += ch2[t2++];
	}

	t1 = 0;
	t2 = ipp2 * t0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1;
		t4 = t2;
		for (integer k = 0; k < l1; k++)
		{
			ch[t3] = c1[t3] - c1[t4];
			ch[t4] = c1[t3] + c1[t4];
			t3 += ido;
			t4 += ido;
		}
	}

	if (ido == 1)
		goto L132;
	if (nbd < l1)
		goto L128;

	t1 = 0;
	t2 = ipp2 * t0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1;
		t4 = t2;
		for (integer k = 0; k < l1; k++)
		{
			t5 = t3;
			t6 = t4;
			for (integer i = 2; i < ido; i += 2)
			{
				t5 += 2;
				t6 += 2;
				ch[t5 - 1] = c1[t5 - 1] - c1[t6];
				ch[t6 - 1] = c1[t5 - 1] + c1[t6];
				ch[t5] = c1[t5] + c1[t6 - 1];
				ch[t6] = c1[t5] - c1[t6 - 1];
			}
			t3 += ido;
			t4 += ido;
		}
	}
	goto L132;

  L128:
	t1 = 0;
	t2 = ipp2 * t0;
	for (integer j = 1; j < ipph; j++)
	{
		t1 += t0;
		t2 -= t0;
		t3 = t1;
		t4 = t2;
		for (integer i = 2; i < ido; i += 2)
		{
			t3 += 2;
			t4 += 2;
			t5 = t3;
			t6 = t4;
			for (integer k = 0; k < l1; k++)
			{
				ch[t5 - 1] = c1[t5 - 1] - c1[t6];
				ch[t6 - 1] = c1[t5 - 1] + c1[t6];
				ch[t5] = c1[t5] + c1[t6 - 1];
				ch[t6] = c1[t5] - c1[t6 - 1];
				t5 += ido;
				t6 += ido;
			}
		}
	}

  L132:
	if (ido == 1)
		return;

	for (integer ik = 0; ik < idl1; ik++)
		c2[ik] = ch2[ik];

	t1 = 0;
	for (integer j = 1; j < ip; j++)
	{
		t2 = (t1 += t0);
		for (integer k = 0; k < l1; k++)
		{
			c1[t2] = ch[t2];
			t2 += ido;
		}
	}

	if (nbd > l1)
		goto L139;

	is = -ido - 1;
	t1 = 0;
	for (integer j = 1; j < ip; j++)
	{
		is += ido;
		t1 += t0;
		integer idij = is;
		t2 = t1;
		for (integer i = 2; i < ido; i += 2)
		{
			t2 += 2;
			idij += 2;
			t3 = t2;
			for (integer k = 0; k < l1; k++)
			{
				c1[t3 - 1] = wa[idij - 1] * ch[t3 - 1] - wa[idij] * ch[t3];
				c1[t3] = wa[idij - 1] * ch[t3] + wa[idij] * ch[t3 - 1];
				t3 += ido;
			}
		}
	}
	return;

  L139:
	is = -ido - 1;
	t1 = 0;
	for (integer j = 1; j < ip; j++)
	{
		is += ido;
		t1 += t0;
		t2 = t1;
		for (integer k = 0; k < l1; k++)
		{
			integer idij = is;
			t3 = t2;
			for (integer i = 2; i < ido; i += 2)
			{
				idij += 2;
				t3 += 2;
				c1[t3 - 1] = wa[idij - 1] * ch[t3 - 1] - wa[idij] * ch[t3];
				c1[t3] = wa[idij - 1] * ch[t3] + wa[idij] * ch[t3 - 1];
			}
			t2 += ido;
		}
	}
}

static void drftb1 (integer n, FFT_DATA_TYPE * c, FFT_DATA_TYPE * ch, FFT_DATA_TYPE * wa, integer *ifac)
{
	const integer nf = ifac[1];
	integer na = 0;
	integer l1 = 1;
	integer iw = 1;

	for (integer k1 = 0; k1 < nf; k1++)
	{
		const integer ip = ifac[k1 + 2];
		const integer l2 = ip * l1;
		const integer ido = n / l2;
		const integer idl1 = ido * l1;
		integer ix2, ix3;
		if (ip != 4)
			goto L103;
		ix2 = iw + ido;
		ix3 = ix2 + ido;

		if (na != 0)
			dradb4 (ido, l1, ch, c, wa + iw - 1, wa + ix2 - 1, wa + ix3 - 1);
		else
			dradb4 (ido, l1, c, ch, wa + iw - 1, wa + ix2 - 1, wa + ix3 - 1);
		na = 1 - na;
		goto L115;

	  L103:
		if (ip != 2)
			goto L106;

		if (na != 0)
			dradb2 (ido, l1, ch, c, wa + iw - 1);
		else
			dradb2 (ido, l1, c, ch, wa + iw - 1);
		na = 1 - na;
		goto L115;

	  L106:
		if (ip != 3)
			goto L109;

		ix2 = iw + ido;
		if (na != 0)
			dradb3 (ido, l1, ch, c, wa + iw - 1, wa + ix2 - 1);
		else
			dradb3 (ido, l1, c, ch, wa + iw - 1, wa + ix2 - 1);
		na = 1 - na;
		goto L115;

	  L109:
		/* The radix five case can be translated later..... */
		/* if(ip!=5)goto L112;

		   ix2=iw+ido; ix3=ix2+ido; ix4=ix3+ido; if(na!=0)
		   dradb5(ido,l1,ch,c,wa+iw-1,wa+ix2-1,wa+ix3-1,wa+ix4-1); else
		   dradb5(ido,l1,c,ch,wa+iw-1,wa+ix2-1,wa+ix3-1,wa+ix4-1); na=1-na;
		   goto L115;

		   L112: */
		if (na != 0)
			dradbg (ido, ip, l1, idl1, ch, ch, ch, c, c, wa + iw - 1);
		else
			dradbg (ido, ip, l1, idl1, c, c, c, ch, ch, wa + iw - 1);
		if (ido == 1)
			na = 1 - na;

	  L115:
		l1 = l2;
		iw += (ip - 1) * ido;
	}

	if (na == 0)
		return;

	for (integer i = 0; i < n; i++)
		c[i] = ch[i];
}

/* End of file NUMfft_core.h */
