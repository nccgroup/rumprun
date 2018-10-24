CFLAGS+=	-Wall -Wextra -Wno-unused-parameter
CFLAGS+=	-Wmissing-prototypes -Wstrict-prototypes
CFLAGS+=	-fstack-protector-strong -mstack-protector-guard=global

CPPFLAGS+=	-I${BMKHEADERS}
