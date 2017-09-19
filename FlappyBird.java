package Flappy;//Package

import javax.swing.JFrame;

public class FlappyBird {
	
	public static FlappyBird flappyBird;
	
	public final int WIDTH = 800, HEIGHT = 800;
	
	public FlappyBird() {
		JFrame jframe = new JFrame();
		
		jframe.setResizable(true);
		jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jframe.setSize(WIDTH, HEIGHT);
		jframe.setVisible(true);
		
	}
	
	//static accessed anywhere
	public static void main(String[] args) {
		
		flappyBird = new FlappyBird();
	}

}
