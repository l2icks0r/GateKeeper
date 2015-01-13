import math, sys

def main():
    
    convert( sys.argv[1] )
    
def convert( ticks_param ):

    ticks = int(ticks_param)

    minutes     = (ticks / 60000)
    seconds     = (ticks / 1000) % 60
    tens_place  = (ticks / 100) % 10
    hund_place  = (ticks / 10 ) % 10
    thou_place  = (ticks) % 10
    

    print "minutes : " + str(minutes)
    print "seconds : " + str(seconds)
    print "tens    : " + str(tens_place)
    print "hund    : " + str(hund_place)
    print "thou    : " + str(thou_place)

    print "------------\n\n"

    print str(minutes) + " : " + str(seconds) + "." + str(tens_place) + str(hund_place) + str(thou_place)
    
if __name__ == '__main__' : main()
