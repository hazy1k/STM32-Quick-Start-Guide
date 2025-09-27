#include <rtthread.h>
#include <rthw.h>

/**
 * 该函数将指定个数的字符串从一个地方拷贝到另外一个地方
 * 
 * @param dst 字符串拷贝的目的地
 * @param src 字符串从哪里拷贝
 * @param n 要拷贝的最大长度
 *
 * @return the result
 */
char *rt_strncpy(char *dst, const char *src, rt_ubase_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }

    return (dst);
}

