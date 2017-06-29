package com.amar.gamecherno;

import java.awt.Canvas;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferStrategy;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;

import javax.swing.JFrame;

import com.amar.gamecherno.graphics.Screen;
import com.amar.gamecherno.input.Keyboard;

public class Game extends Canvas implements Runnable {
	private static final long serialVersionUID = 1L;
	
	public static int width = 300;
	public static int height = width / 16*9;
	public static int scale = 3;
	public static String title = "Game";
	
	private Thread thread;
	private JFrame frame;
	private Keyboard key;
	private boolean running  = false;
	
	private Screen screen;
	
	//This creates a image
	private BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
	//This allows us to create/draw inside the image.
	private int [] pixels = ((DataBufferInt)image.getRaster().getDataBuffer()).getData();
	
	public Game() {
		Dimension size = new Dimension(width * scale, height * scale);
		setPreferredSize(size);
		
		screen = new Screen(width, height);
		frame = new JFrame();
		key = new Keyboard();
		
		addKeyListener(key);
	}
	// WHAT THIS DOES IS IT STARTS THE APPLICATION
	public synchronized void start() {
		running  = true;
		thread = new Thread(this, "Display");
		thread.start();
	}
	
	//AND THIS STOPS THE APPLICATION
	public synchronized void stop(){
		running = false;
		try {
		thread.join();
		}catch(InterruptedException e){
			e.printStackTrace();
		}
	}
	// This keeps the game running(running == true)
	public void run(){
		long lastTime = System.nanoTime();
		long timer = System.currentTimeMillis();
		final double nanosec = 1000000000.0/ 60.0;
		double delta = 0;
		int frames = 0;
		int updates = 0; 
		while (running == true){
			long now = System.nanoTime();
			delta += (now - lastTime)/ nanosec;
			lastTime = now;
			while (delta >= 1) {
				update();
				updates++;
				delta--;
			}
			render();
			frames++;
			
			if (System.currentTimeMillis() - timer > 1000) {
				timer +=1000;
				System.out.println(updates + " ups, " + frames + "fps");
				frame.setTitle(title + "  |   " + updates + " ups, " + frames + "fps");
				updates = 0;
				frames = 0;
			
			}
		}
		stop();
	}
	int x=0, y=0;
	public void update(){
		key.update();
		if(key.up) y--;
		if(key.down) y++;
		if(key.left) x--;
		if(key.right) x++;
		
		
	}
	public void render(){
		BufferStrategy bs = getBufferStrategy();
		if (bs == null) {
			createBufferStrategy(3);
			return;
		}
		screen.clear();
		screen.render(x, y);
		 
		for (int i = 0; i < pixels.length; i++){
			pixels[i] = screen.pixels[i];
		}
		
		Graphics g = bs.getDrawGraphics();
		g.setColor(Color.BLACK);
		g.fillRect(0, 0, getWidth(), getHeight());
		g.drawImage(image, 0, 0, getWidth(), getHeight(), null);
		g.dispose();
		bs.show();
	}
	public static void main(String [] args) {
		Game game = new Game();
		game.frame.setResizable(false);
		game.frame.setTitle(Game.title);
		game.frame.add(game);
		game.frame.pack();
		game.frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		game.frame.setLocationRelativeTo(null);
		game.frame.setVisible(true);
		
		game.start();
	}

}
