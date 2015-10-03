import sys, math

def main():

    array_string = ''

    char_count = 0

    mask = 0
    angle = int(sys.argv[ 2 ])
    theta = int(sys.argv[ 3 ])

    if len(sys.argv[1]) > 20:
        print 'string is > 20 characters'
        exit(0)

    print '// "' + sys.argv[ 1 ] + '"'

    for letter in sys.argv[ 1 ]:

        angle += theta
        
        mask = 32767 * math.sin( angle )

        array_string += str( ord( letter ) ^ int( mask ) ) + ', '
        char_count += 1

    print 'static const short ' + sys.argv[ 4 ] + '[ ' + str(char_count + 3) + ' ] = { ' + sys.argv[ 2 ] + ', ' + sys.argv[ 3 ] + ', ' + array_string[:-2] + ', 0 };'


if __name__ == '__main__' : main()
