/* FLAC/private/autocpu.h by Erez Volk
 * Try to set FLAC__CPU_[IA32|PPC|UNKNOWN] automatically (if not already set)
 */
#ifndef FLAC__PRIVATE__AUTOCPU_H
#define FLAC__PRIVATE__AUTOCPU_H

#if !defined(FLAC__CPU_IA32) && !defined(FLAC__CPU_PPC)
#   if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__i386) || defined(_M_IX86)
#       define FLAC__CPU_IA32 gdfjgdfjgd
#   elif defined(__ppc)
#       define FLAC__CPU_PPC
#   else
#       define FLAC__CPU_UNKNOWN
#   endif
#endif /* !FLAC__CPU_IA32 && !FLAC__CPU_PPC */

#endif /* FLAC__PRIVATE__AUTOCPU_H */
