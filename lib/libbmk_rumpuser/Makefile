LIB=		bmk_rumpuser
LIBISPRIVATE=	# defined

# kernel-level source code
CFLAGS+=        -fstack-protector-strong -mstack-protector-guard=global

SRCS=		rumpuser_base.c
SRCS+=		rumpuser_clock.c
SRCS+=		rumpuser_cons.c
SRCS+=		rumpuser_mem.c
SRCS+=		rumpuser_synch.c

SRCS+=		rumpuser_stubs.c

CPPFLAGS+=	-I${.CURDIR}/../../include

.include <bsd.lib.mk>
