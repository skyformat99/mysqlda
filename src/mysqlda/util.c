#include "mysqlda_in.h"

/* 写数据覆盖到文件 */
int WriteEntireFile( char *pathfilename , char *file_content , int file_len )
{
	FILE		*fp = NULL ;
	size_t		size ;
	
	fp = fopen( pathfilename , "wb" ) ;
	if( fp == NULL )
	{
		return -1;
	}
	
	size = fwrite( file_content , 1 , file_len , fp ) ;
	if( size != file_len )
	{
		fclose( fp );
		return -2;
	}
	
	fclose( fp );
	
	return 0;
}

/* 读取文件，内容存放到动态申请内存块中 */
/* 注意：使用完后，应用负责释放内存 */
char *StrdupEntireFile( char *pathfilename , int *p_file_len )
{
	char		*file_content = NULL ;
	int		file_len ;
	FILE		*fp = NULL ;
	
	int		nret = 0 ;
	
	fp = fopen( pathfilename , "rb" ) ;
	if( fp == NULL )
	{
		return NULL;
	}
	
	fseek( fp , 0 , SEEK_END );
	file_len  = ftell( fp ) ;
	fseek( fp , 0 , SEEK_SET );
	
	file_content = (char*)malloc( file_len+1 ) ;
	if( file_content == NULL )
	{
		fclose( fp );
		return NULL;
	}
	memset( file_content , 0x00 , file_len+1 );
	
	nret = fread( file_content , 1 , file_len , fp ) ;
	if( nret != file_len )
	{
		fclose( fp );
		free( file_content );
		return NULL;
	}
	
	fclose( fp );
	
	if( p_file_len )
		(*p_file_len) = file_len ;
	return file_content;
}

/* 转换当前进程为守护进程 */
int BindDaemonServer( int (* ServerMain)( void *pv ) , void *pv , int close_flag )
{
	int	pid ;
	
	pid = fork() ;
	switch( pid )
	{
		case -1:
			return -1;
		case 0:
			break;
		default		:
			return 0;
	}
	
	pid = fork() ;
	switch( pid )
	{
		case -1:
			return -2;
		case 0:
			break ;
		default:
			return 0;
	}
	
	if( close_flag )
	{
		close(0);
		close(1);
		close(2);
	}
	
	umask( 0 ) ;
	
	chdir( "/tmp" );
	
	ServerMain( pv );
	
	return 0;
}

/* 产生二进制随机串 */
void GenerateRandomDataWithoutNull( char *data , int data_len )
{
	int	i ;
	char	*p = NULL ;
	
	srand( time(NULL) );
	
	for( i = 0 , p = data ; i < data_len ; i++ , p++ )
	{
		(*p) = (char)rand() ;
		if( (*p) == '\0' )
			p--;
	}
	
	return;
}

/* 计算字符串的哈希值 */
unsigned long CalcHash( char *str , int str_len )
{
	char		*p = str ;
	char		*p_end = str + str_len - 1 ;
	unsigned long	ul = 0 ;
	
	for( ; p <= p_end ; p++ )
	{
		ul  = (*p) + (ul<<6)+ (ul>>16) - ul ;
	}
	
	return ul;
}

/* 比较多词字符串前缀，如果前缀相同则返回比较偏移地址，否则返回NULL */
char *wordncasecmp( char *s1 , char *s2 , size_t n )
{
	char	*p = NULL ;
	
	if( s1 == NULL )
		return NULL;
	
	for( p = s1 ; (*p) && (*p)==' ' && (*p)=='\t' ; p++ )
		;
	if( (*p) == '\0' )
		return NULL;
	
	if( strncasecmp( p , s2 , n ) )
		return NULL;
	
	return p+n;
}

