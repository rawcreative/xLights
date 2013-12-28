	'use strict';
	module.exports = function(grunt) {

		// load all grunt tasks
		require('matchdep').filterDev('grunt-*').forEach(grunt.loadNpmTasks);

		grunt.initConfig({
			pkg: grunt.file.readJSON('package.json'),
			fldrName: '<%= pkg.name.charAt(0).toUpperCase() + pkg.name.slice(1) %>',
		
			watch: {
			
				compass: {
					files: ['assets/sass/**/*.{scss,sass}'],
					tasks: ['compass']
				},
				js: {
					files: '<%= jshint.all %>',
					tasks: ['jshint', 'uglify']
				}
			},

			// compass and scss
			compass: {
				dist: {
					options: {
						sassDir: 'assets/sass',
						cssDir: 'assets/css',
						imagesDir: 'assets/img',
						javascriptsDir: 'assets/js',
						fontsDir: 'assets/fonts',
						force: true,
						environment: 'production',
						noLineComments: true
					}
				}
				
			},

			// javascript linting with jshint
			jshint: {
				options: {
					jshintrc: '.jshintrc',
					"force": true
				},
				all: [
						'Gruntfile.js', 
						'assets/js/**/*.js']
			},

			// uglify to concat, minify, and make source maps
			uglify: {
				dist: {
					options: {
						mangle: true,
						sourceMap: 'assets/js/map/source-map.js'
					},
					files: {
						// 'assets/js/plugins.min.js': ['assets/js/plugins/**/*.js', '!assets/js/plugins/modernizr*.js'],
						'assets/js/scripts.min.js': [
							'assets/js/plugins/modernizr-2.6.2.js',
							'assets/js/plugins/utils.js',
							'assets/js/plugins/imagesloaded.js',
							'assets/js/plugins/easing.js',
							'assets/js/plugins/transit.js',
							'core/Rawfolio/js/rg.js',
							'assets/js/plugins/lazyload.js',
							'assets/js/plugins/scrollbars.js',
							'assets/js/plugins/scrollstart.js',
							'assets/js/plugins/placeholder.js',
							'assets/js/plugins/validation.js',
							'assets/js/plugins/fitvids.js',
							'assets/js/plugins/spin.js',
							'assets/js/plugins/ladda.js',
							'assets/js/src/custom.js',
							'core/Rawfolio/js/modules/proofing.js',
							'assets/js/modules/*.js']
					}
				}
			},

			// image optimization
			imagemin: {
				dist: {
					options: {
						optimizationLevel: 7,
						progressive: true
					},
					files: [{
						expand: true,
						cwd: 'build/<%= fldrName %>/assets/img/',
						src: '**/*',
						dest: 'build/<%= fldrName %>/assets/img/'
					},
					{
						expand: true,
						cwd: 'build/<%= fldrName %>/core/Rawfolio/Admin/assets/img/',
						src: '**/*',
						dest: 'build/<%= fldrName %>/core/Rawfolio/Admin/assets/img/'
					}]
				}
			},

			bump: {
			  options: {
			    files: ['package.json', 'index.html'],
			    updateConfigs: [],
			    commit: false,
			    createTag: false,
			    tagName: 'v%VERSION%',
			    push: false
			  	  
			  }
			}

			
		});

		// rename tasks
		//grunt.renameTask('regarde', 'watch');
		grunt.registerTask('default', ['watch']);
	
	};