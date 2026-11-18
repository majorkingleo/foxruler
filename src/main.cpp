#include "fx.h"


class Main : public FXMainWindow
{
  FXDECLARE( Main );

public:
  enum 
	{
	  ID_ORIENTATION = FXMainWindow::ID_LAST,	  
	  ID_MAINWINDOW,
	  ID_ABOUT,
	  ID_QUIT,
	  ID_LAST
	};

private:
  FXFont *font;
  FXFont *font2;

  enum ORIENTATION
	{
	  VERTICAL,
	  HORIZONTAL
	};

  ORIENTATION orientation;
  bool mouse_down;
  int last_x;
  int last_y;
  bool dirty;
  int last_h_height;
  bool dragging;
  int drag_x;
  int drag_y;

private:
  Main()=default;

public:
  Main( FXApp *app )
	: FXMainWindow( app, "FoxRuler", nullptr, nullptr, DECOR_NONE, 
					0, 0, 500, 50 )
  {
	orientation = HORIZONTAL;
	mouse_down = false;
	last_x = -1;
	last_y = -1;
	dirty = false;
	last_h_height = height;
	dragging = false;
	drag_x = 0;
	drag_y = 0;

	font = new FXFont( app, "helvetica", 10 );
	font2 = new FXFont( app, "helvetica", 8,  FXFont::Bold );

	setTarget(this);
	setSelector(ID_MAINWINDOW);
  }

  ~Main() override
  {
	delete font;
	delete font2;
  }
 
  void readRegistry()
  {
	// Position
	FXint xx=getApp()->reg().readIntEntry("SETTINGS","x",50);
	FXint yy=getApp()->reg().readIntEntry("SETTINGS","y",50);
	FXint ww=getApp()->reg().readIntEntry("SETTINGS","w",500);
	FXint hh=getApp()->reg().readIntEntry("SETTINGS","h",50);
	FXint oo=getApp()->reg().readIntEntry("SETTINGS","orientation",HORIZONTAL);
	FXint h_height=getApp()->reg().readIntEntry("SETTINGS","last_h_height",50);

	if( xx < 0 )
	  xx = 50;
	if( yy < 0 )
	  yy = 50;

	if( xx > getApp()->getRootWindow()->getWidth() )
	  xx =  0;
	
	if( yy > getApp()->getRootWindow()->getHeight() )
	  yy =  0;

	if( xx + ww > getApp()->getRootWindow()->getWidth() )
	  ww = getApp()->getRootWindow()->getWidth() - xx;

	if( yy + hh > getApp()->getRootWindow()->getHeight() )
	  hh = getApp()->getRootWindow()->getHeight() - yy;

	if( ww <= 50 )
	  ww = 500;
	
	if( hh <= 50 )
	  hh = 50;

	if( h_height < 50 )
	  h_height = 50;
   
	if( h_height + yy > getApp()->getRootWindow()->getHeight() )
	  h_height = 50;
	
	last_h_height = h_height;

	setX(xx);
	setY(yy);
	setWidth(ww);
	setHeight(hh);

	switch( oo )
	  {
	  case HORIZONTAL:
	  case VERTICAL:
		orientation = static_cast<ORIENTATION>(oo);
		break;
	  }

  }

  void writeRegistry()
  {
	// Position
	getApp()->reg().writeIntEntry("SETTINGS","x",getX());
	getApp()->reg().writeIntEntry("SETTINGS","y",getY());
	getApp()->reg().writeIntEntry("SETTINGS","w",getWidth());
	getApp()->reg().writeIntEntry("SETTINGS","h",getHeight());   
	getApp()->reg().writeIntEntry("SETTINGS", "orientation", orientation );
	getApp()->reg().writeIntEntry("SETTINGS", "last_h_height", last_h_height );
  }


#if defined(FOX_X11)
  // Bypass the window manager so the ruler stays free of decorations.
  FXbool doesOverrideRedirect() const override
  {
	return true;
  }
#endif

  FXbool close( FXbool notify) override
  {
	writeRegistry();
	return FXMainWindow::close( notify );
  }

  void create() override
  {
	readRegistry();

	FXMainWindow::create();
	font->create();	
	font2->create();

	show();

	// for getting mouse messages
	enable();
  }

  

  long onPaint( FXObject*, FXSelector, void* )
  {
	FXDCWindow dc( this );

	dc.setForeground( FXRGB( 255, 255, 0 ) );
	dc.fillRectangle( 0, 0, width, height );

	dc.setForeground( FXRGB( 0, 0, 0 ) );

	int w = width;

	if( orientation == VERTICAL )
	  w = height;

	for( int i = 0; i < w; i += 5 )
	  {
		int h;

		if( i % 50 == 0 )	   
		  h = 10;
		else if( i % 10 == 0 )	   
		  h = 7;
		else
		  h = 5;

		if( orientation == VERTICAL )
		  {
			dc.drawLine( 0, i, h, i );
			dc.drawLine( width - h, i, width , i );
		  } else {
			dc.drawLine( i, 0, i, h );
			dc.drawLine( i, height - h, i, height );
		  }

		if( i % 50 == 0 )	   
		  {
			FXString s;
			s.format( "%d", i );
			dc.setFont( font );

			if( orientation == VERTICAL )
			  {
				dc.drawText( h + 3,
							 i + font->getFontDescent() + 1,
							 s.text(),
							 s.length() );				

				if( width > 50 )
				  {
					dc.drawText( width - h - font->getTextWidth( s ) - 2,
								 i + font->getFontDescent() + 1,
								 s.text(),
								 s.length() );				
				  }

			  } else {
				dc.drawText( i - font->getTextWidth( s ) / 2 , 
							 h + 1 + font->getTextHeight( s ), 
							 s.text(), 
							 s.length() );

				if( height > 50 )
				  {
					dc.drawText( i - font->getTextWidth( s ) / 2 , 
								 height - h - font->getFontDescent(),
								 s.text(),
								 s.length() );

				  }
			  }
		  }
	  }
	
	return 1;
  }

  long onOrientation( FXObject*, FXSelector, void* )
  {
	int w = width, h = height;

	if( orientation == HORIZONTAL )
	  {
		orientation = VERTICAL;
		last_h_height = height;
	 } else {
		orientation = HORIZONTAL;	
		w = last_h_height;
	  }

	resize( h, w );

	return 1;
  }

  long onMouseDown( FXObject*, FXSelector, void* )
  {
	mouse_down = true;
	last_x = -1;
	last_y = -1;
	return 1;
  }

  long onMouseUp( FXObject*, FXSelector, void* ptr )
  {
	mouse_down = false;

	if( dirty )
	  {

		FXDCWindow dc(this);
		FXEvent *ev=(FXEvent*)ptr;
		dc.setFunction( BLT_NOT_DST );	
		
		switch( orientation )
		  {
		  case HORIZONTAL:
			drawText( -1 );
			dc.setFunction( BLT_NOT_DST );	
			dc.drawLine( ev->win_x, 0, ev->win_x, height );
			break;
			
		  case VERTICAL:	   
			drawText( -1 );
			dc.setFunction( BLT_NOT_DST );	
			dc.drawLine( 0, ev->win_y, width, ev->win_y );		
			break;
		  };
	  }
		
	last_x = -1;
	last_y = -1;
	dirty = false;

	return 1;
  }

  void drawText( int number )
  {
	FXString s;
	FXDCWindow dc( this );

	s.format( "% 5d", number );
	dc.setFunction();
	dc.setFont( font2 );
	dc.setForeground( FXRGB( 255, 255, 0 ) );

	int x=0, y=0, w, h;

	switch( orientation )
	  {
	  case HORIZONTAL:
		x = 40 - font->getTextWidth( s );
		y = height / 2 - font->getTextHeight( s ) / 2;
		break;

	  case VERTICAL:
		x = width / 2 - font->getTextWidth( s ) /2;
		y = 40 - font->getTextHeight( s );
		break;
	  }

	w = font->getTextWidth( s );
	h = font->getTextHeight( s );

	dc.fillRectangle( x, y, w, h );

	if( number < 0 )
	  return;

	dc.setForeground( FXRGB( 0, 0, 0 ) );

	dc.drawText( x, 
				 y + font->getFontAscent(),
				 s.text(),
				 s.length() );	
  }

  long onMiddleButtonPress( FXObject*, FXSelector, void* ptr )
  {
	FXEvent *ev=(FXEvent*)ptr;
	dragging = true;
	drag_x = ev->root_x - getX();
	drag_y = ev->root_y - getY();
	raise();
	startDrag();
	grab();
	return 1;
  }

  long onMiddleButtonRelease( FXObject*, FXSelector, void* )
  {
	dragging = false;
	ungrab();
	return 1;
  }

  long onUngrabbed( FXObject*, FXSelector, void* )
  {
	dragging = false;
	return 1;
  }

  long onMouseMove( FXObject*, FXSelector, void* ptr )
  {
	FXEvent *ev=(FXEvent*)ptr;

	if( dragging )
	  {
		move( ev->root_x - drag_x, ev->root_y - drag_y );
		return 1;
	  }

	if( !mouse_down )
	  return 0;

	FXDCWindow dc(this);

	dc.setFunction( BLT_NOT_DST );
	dc.setFont( font );

	dirty = true;

	switch( orientation )
	  {
	  case HORIZONTAL:
		{
		  dc.drawLine( last_x, 0, last_x, height );
		  dc.drawLine( ev->win_x, 0, ev->win_x, height );
		  drawText( ev->win_x );
		}
		break;

	  case VERTICAL:	   
		{
		  dc.drawLine( 0, last_y, width, last_y );		
		  dc.drawLine( 0, ev->win_y, width, ev->win_y );		
		  drawText( ev->win_y );
		}
		break;
	  };

	last_x = ev->win_x;
	last_y = ev->win_y;

	return 1;
  }

  long onContextMenu( FXObject*, FXSelector, void* ptr )
  {
	FXEvent *event=(FXEvent*)ptr;
	if(event->moved) 
	  return 1;

	FXMenuPane pane( this );

	FXString s;

	if( orientation == HORIZONTAL )
	  s = "Orientation Vertical";
	else
	  s = "Orientation Horizontal";

	new FXMenuCommand( &pane, s, nullptr, this, ID_ORIENTATION );
	new FXMenuCommand( &pane, "About", nullptr, this, ID_ABOUT );
	new FXMenuCommand( &pane, "Quit", nullptr, this, ID_QUIT );

	pane.create();
	pane.popup(nullptr,event->root_x,event->root_y);
	getApp()->runModalWhileShown(&pane);
	return 1;
  }

  long onAbout( FXObject*, FXSelector, void* )
  {
	FXMessageBox::information(this,MBOX_OK,"About FOXRuler","FOXRuler\n(C) 2005 by Martin Oberzalek <kingleo@gmx.at>\nUnder the terms of the GPL.");

	return 1;
  }

  long onQuit( FXObject*, FXSelector, void* )
  {
	close( true );
	return 1;
  }

};

FXDEFMAP( Main ) MainMap[] = {
  FXMAPFUNC( SEL_PAINT, 0, Main::onPaint ),
  FXMAPFUNC( SEL_COMMAND, Main::ID_ORIENTATION, Main::onOrientation ),
  FXMAPFUNC( SEL_COMMAND, Main::ID_ABOUT, Main::onAbout ),
  FXMAPFUNC( SEL_COMMAND, Main::ID_QUIT, Main::onQuit ),
  FXMAPFUNC( SEL_MOTION, Main::ID_MAINWINDOW, Main::onMouseMove ),
  FXMAPFUNC( SEL_MIDDLEBUTTONPRESS, Main::ID_MAINWINDOW, Main::onMiddleButtonPress ),
  FXMAPFUNC( SEL_MIDDLEBUTTONRELEASE, Main::ID_MAINWINDOW, Main::onMiddleButtonRelease ),
  FXMAPFUNC( SEL_UNGRABBED, Main::ID_MAINWINDOW, Main::onUngrabbed ),
  FXMAPFUNC( SEL_LEFTBUTTONPRESS, Main::ID_MAINWINDOW, Main::onMouseDown ),
  FXMAPFUNC( SEL_LEFTBUTTONRELEASE, Main::ID_MAINWINDOW, Main::onMouseUp ),
  FXMAPFUNC( SEL_RIGHTBUTTONRELEASE, Main::ID_MAINWINDOW, Main::onContextMenu )
};

FXIMPLEMENT( Main, FXMainWindow, MainMap, ARRAYNUMBER( MainMap ) );


int main( int argc, char **argv )
{
  FXApp app( "FoxRuler", "FoxRuler" );

  app.init( argc, argv );

  
  new Main( &app );

  app.create();

  return app.run();
}
