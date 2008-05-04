#ifndef common_defs_h_INCLUDED
#define common_defs_h_INCLUDED

typedef long TickerId;
typedef long OrderId;

enum faDataType { GROUPS=1, PROFILES, ALIASES } ;

inline char * faDataTypeStr ( faDataType pFaDataType )
{
	switch (pFaDataType) {
		case GROUPS:
			return "GROUPS" ;
			break ;
		case PROFILES:
			return "PROFILES" ;
			break ;
		case ALIASES:
			return "ALIASES" ;
			break ;
	}
	return NULL ;
}

#endif /* common_defs_h_INCLUDED */
