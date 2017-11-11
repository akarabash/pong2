/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>

#define GREEN_LED BIT6

//these variables are used to keep score of each player and if the game is still going on or not
int redScore = 0;
int blueScore = 0;
int gameOver = 1;

AbRect rect10 = {abRectGetBounds, abRectCheck, {15,1}}; /**< paddles size set to 15x1 */

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer player2 = {               //this is the layer of the paddle for player 2
  (AbShape *)&rect10,
  {102, 148},                   //start position of the paddle 
  {0,0},{0,0},				    /* last & next pos */
  COLOR_BLUE,
  0
};
  

Layer layer3 = {		/**< Layer of the ball for pong*/
  (AbShape *)&circle3,
  {(screenWidth/2)+5, (screenHeight/2)+10}, /**< start position of the ball, middle of the screen */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &player2,
};


Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer3
};

Layer player1 = {		/**< this is the layer which holds the paddle of player 1 */
  (AbShape *)&rect10,
  {26,12},              //start position of the paddle 
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */
MovLayer ml4 = { &player2, {0,0}, 0 }; /**< not all layers move */
MovLayer ml3 = { &layer3, {2,5}, &ml4 }; 
MovLayer ml1 = { &player1, {0,0}, &ml3 };  /**< not all layers move */

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence, Region *redBar, Region *blueBar)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
      
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    
    if ((shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[0]) || //rewrote of if statements to make the ball
        (shapeBoundary.botRight.axes[0] > fence->botRight.axes[0])){ //bounce of the walls
	int velocity = ml->velocity.axes[0] = -ml->velocity.axes[0];
	newPos.axes[axis] += (2*velocity);
    P2DIR = BIT6;
    buzzer_set_period(4000);
    }
    
    if ((shapeBoundary.topLeft.axes[1] < fence->topLeft.axes[1]) || //if the ball touche the top or botton wall
        (shapeBoundary.botRight.axes[1] > fence->botRight.axes[1])){ //the ball resets in the middle of the 
                                                                    //screen
        newPos.axes[1] = (screenHeight/2)+5;
        newPos.axes[0] = (screenWidth/2)+10;
        }
        
    if(shapeBoundary.topLeft.axes[1] < fence->topLeft.axes[1]){ //these if statements update the score of each
        blueScore++;                                            //player
    }
    if(shapeBoundary.botRight.axes[1] > fence->botRight.axes[1]){
        redScore++;
    }
        
   
    
    /*the if statements below make the ball bounce f the ball hits the red or blue paddle
     */
    if(shapeBoundary.topLeft.axes[1]<redBar->botRight.axes[1] && shapeBoundary.botRight.axes[0]>redBar->topLeft.axes[0] && shapeBoundary.topLeft.axes[0]<redBar->botRight.axes[0]){
        int velocity = ml->velocity.axes[1] = -ml->velocity.axes[1];
    }
    
    if(shapeBoundary.botRight.axes[1]>blueBar->topLeft.axes[1] && shapeBoundary.topLeft.axes[0]<blueBar->botRight.axes[0] && shapeBoundary.botRight.axes[0]>blueBar->topLeft.axes[0]){
        int velocity = ml->velocity.axes[1] = -ml->velocity.axes[1];
    }
      
      /**< if outside of fence */
      /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}
/*
 the buzzer_init method below is used to generate the buzzer sound in the msp430 board
 */
void buzzer_init()
{
    /* 
       Direct timer A output "TA0.1" to P2.6.  
        According to table 21 from data sheet:
          P2SEL2.6, P2SEL2.7, anmd P2SEL.7 must be zero
          P2SEL.6 must be 1
        Also: P2.6 direction must be output
    */
    timerAUpmode();		/* used to drive speaker */
    P2SEL2 &= ~(BIT6 | BIT7);
    P2SEL &= ~BIT7; 
    P2SEL |= BIT6;
    P2DIR = BIT6;		/* enable output to speaker (P2.6) */

}

void buzzer_set_period(short cycles){
    CCR0 = cycles; 
    CCR1 = cycles >> 1;		/* one half cycle */
}


u_int bgColor = COLOR_WHITE;/**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */
Region player1Fence; 
Region player2Fence;


/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(15);

  shapeInit();

  layerInit(&player1);
  layerDraw(&player1);
  
  //added this method call to print the player 1 and player 2 titles on the lcd screen
  drawString5x7(1,1, "player 1", COLOR_RED, COLOR_WHITE);
  drawString5x7(80,152, "player 2", COLOR_BLUE, COLOR_WHITE);
  //drawString5x7(screenWidth/2,screenHeight/2, "Who Dis", COLOR_LIME_GREEN, COLOR_WHITE);


  buzzer_init();
  layerGetBounds(&fieldLayer, &fieldFence);
  layerGetBounds(&player1, &player1Fence);
  layerGetBounds(&player2, &player2Fence);


  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml1, &player1);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec 
    added if statements to the Watchdog timer interrupt which determine which buttons are pressed and when,
    the buttons are used to move the paddles and also determine when the game is over.
 */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  
  if(gameOver == 1){
  if (count == 15) {
      buzzer_set_period(0);
      mlAdvance(&ml1, &fieldFence, &player1Fence, &player2Fence);
      u_int switches = p2sw_read(), i;
      layerGetBounds(&player1, &player1Fence);
      layerGetBounds(&player2, &player2Fence);
       char str[5];
    
  for(i = 0;i < 4; i++){
      str[i] = (switches & (1<<i)) ? 0 : 1;
  }
  
  // dtermines which buttons are being pressed and moves the red or blue paddle
  str[4]=0;
    if (str[0]){
        ml1.velocity.axes[0] = -5; 
        ml1.velocity.axes[1] = 0;
    } if (str[1]){
        ml1.velocity.axes[0] = 5; 
        ml1.velocity.axes[1] = 0;
    } if(str[2]){
        ml4.velocity.axes[0] = -5;
        ml4.velocity.axes[1] = 0;
    } if(str[3]){
        ml4.velocity.axes[0] = 5;
        ml4.velocity.axes[1] = 0;
    } if(!str[0] && !str[1]){
        ml1.velocity.axes[0] = 0;
        ml1.velocity.axes[1] = 0;
    } if(!str[2] && !str[3]){
        ml4.velocity.axes[0] = 0;
        ml4.velocity.axes[1] = 0;
    } 
    
    //if statements that determine the score in the game, and when the game is finished or not
    if(redScore == 0 && blueScore == 0){
        drawString5x7(70,1, "Score: 0", COLOR_RED, COLOR_WHITE);
        drawString5x7(1,152, "Score: 0", COLOR_BLUE, COLOR_WHITE);
    } if(redScore == 1){
        drawString5x7(70,1, "Score: 1", COLOR_RED, COLOR_WHITE);
    } if(redScore == 2){
        drawString5x7(70,1, "Score: 2", COLOR_RED, COLOR_WHITE);
    } if(redScore == 3){
        drawString5x7(70,1, "Score: 3", COLOR_RED, COLOR_WHITE);
    } if(redScore == 4){
        drawString5x7(70,1, "Score: 4", COLOR_RED, COLOR_WHITE);
        gameOver = 0;
    } if(blueScore == 1){
        drawString5x7(1, 152, "Score: 1", COLOR_BLUE, COLOR_WHITE);
    } if(blueScore == 2){
        drawString5x7(1, 152, "Score: 2", COLOR_BLUE, COLOR_WHITE);
    } if(blueScore == 3){
        drawString5x7(1, 152, "Score: 3", COLOR_BLUE, COLOR_WHITE);
    } if(blueScore == 4){
        drawString5x7(1, 152, "Score: 4", COLOR_BLUE, COLOR_WHITE);
        gameOver = 0;
    }
     //redrawScreen = 1;
    if(p2sw_read())
        redrawScreen = 1;
    
    //game over print for the lcd screen
    count = 0;
  } 
} else if(redScore == 4){
      drawString5x7(screenHeight/2-44,screenWidth/2, "GAME OVER", COLOR_BLACK, COLOR_WHITE);
      drawString5x7(screenHeight/2-50,screenWidth/2+20, "PLAYER 1 WINS", COLOR_BLACK, COLOR_WHITE);
      //clearScreen(COLOR_BLACK);
      //buzzer_init(0);
      clearScreen(COLOR_WHITE);
START();
} else if(blueScore == 4){
    
      drawString5x7(screenHeight/2-44,screenWidth/2, "GAME OVER", COLOR_BLACK, COLOR_WHITE);
      drawString5x7(screenHeight/2-50,screenWidth/2+20, "PLAYER 2 WINS", COLOR_BLACK, COLOR_WHITE);
        //clearScreen(COLOR_BLACK);
        //buzzer_init(0);
      clearScreen(COLOR_WHITE);
START();
}
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}

