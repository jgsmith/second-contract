$(function() {

SC = { };

_.templateSettings = {
  interpolate: /\{\{(.+?)\}\}/g
};

/*
 * Models and Collections for various resources
 */

SC.Collections = { };
SC.Models = { };

SC.Models.System = Backbone.Model.extend({
  url: "http://localhost:6050/api/system",
});

SC.Models.system = new SC.Models.System;


SC.Models.Verb = Backbone.Model.extend({
  defaults: function() {
    return {
    };
  },

  urlRoot: "http://localhost:6050/api/iflib/verb",

  url: function() {
    var base = this.urlRoot || (this.collection && this.collection.url) || "/";
    if(this.isNew()) return base;

    return base + "/" + encodeURIComponent(this.id);
  },

  initialize: function() {
  },

  validate: function(attributes) {
    if(!attributes.verbs || !$.isArray(attributes.verbs) || attributes.verbs.length == 0) {
      return "A verb must have at least one word to identify it.";
    }
  }
});

SC.Collections.Verb = Backbone.Collection.extend({
  model: SC.Models.Verb,
  url: 'http://localhost:6050/api/iflib/verb',
});

SC.Collections.verbs = new SC.Collections.Verb;

SC.Collections.verbs.fetch();

SC.Views = { };

SC.Views.Verb = Backbone.View.extend({
  initialize: function() {
    this.template = _.template($('#verb-template').html());
    this.listenTo(this.model, 'change', this.render);
  },

  render: function() {
    this.$el.html(this.template(this.model.toJSON()));
    return this.el;
  },

  events: {
    'click .save': 'save'
  },

  save: function() {
    this.model.set({
      verbs: $('#verb-verbs').split(/\s*,\s*/),
      help: $('#verb-help')
    });

    if(this.model.isNew()) {
      var self = this;
      SC.Collections.verbs.create(this.model, {
        success: function() {
        }
      });
    }
    else {
      this.model.save();
    }
  
    return false;
  }
});

SC.Views.VerbList = Backbone.View.extend({
  tagName: "ul",

  initialize: function() {
    this.model.bind( 'reset', this.render, this);
    this.model.bind( 'add', this.appendNewVerb, this);
  },

  render: function() {
    _.each(this.model.models, function(verb) {
      this.appendNewVerb(verb);
    }, this);
    return this.el;
  },

  appendNewVerb: function(verb) {
    this.$el.append(new SC.Views.VerbListItem({model:verb}).render());
  }
});

SC.Views.VerbListItem = Backbone.View.extend({
  tagName: 'li',

  initialize: function() {
    this.template = _.template( $("#verb-list-item-template").html() );
    this.model.bind('change', this.render(), this);
  },

  render: function() {
    this.$el.html( this.template( this.model.toJSON() ) );
    return this.el;
  }
});

SC.Views.Header = Backbone.View.extend({
  initialize: function() {
    this.template = _.template( $("#header-template").html() );
  },

  render: function() {
    this.$el.html( this.template() );
    return this.el;
  },

  events: {
  }
});
  
SC.Views.System = Backbone.View.extend({
  initialize: function() {
    this.template = _.template( $("#system-template").html() );
  },

  events: {
    "click #save-system-name": "saveSystemInfo"
  },

  saveSystemInfo: function() {
    this.model.set({
      name: $("#system-name").val()
    });
    this.model.save();
  },

  render: function() {
    console.log($("#system-template").html());
    this.$el.html( this.template(this.model.toJSON()) );
    return this.el;
  }
});

SC.Views.IFLib = Backbone.View.extend({
  initialize: function() {
    this.template = _.template( $("#iflib-template").html() );
  },

  render: function() {
    this.$el.html( this.template() );
    return this.el;
  }

});

SC.Views.WorldLib = Backbone.View.extend({
  initialize: function() {
    this.template = _.template( $("#worldlib-template").html() );
  },

  render: function() {
    this.$el.html( this.template() );
    return this.el;
  }
});

SC.Router = Backbone.Router.extend({
  routes : {
    "": "system",
    "verb": "verbs",
    "verb/:id": "verb",
    "iflib": "iflib",
    "worldlib": "worldlib"
  },
  system: function() {
    this.showView("#main", new SC.Views.System({model: SC.Models.system }));
    this.showNav("");
  },
  iflib: function() {
    this.showView("#main", new SC.Views.IFLib());
    this.showNav("iflib");
  },
  worldlib: function() {
    this.showView("#main", new SC.Views.WorldLib());
    this.showNav("worldlib");
  },
  verbs: function() {
    this.showView("#main", new SC.Views.VerbList({model: SC.Collections.verbs }));
    this.showNav("iflib");
  },
  verb: function(id) {
    this.showView("#main", new SC.Views.Verb({model: SC.Collections.verbs.get(id)}));
    this.showNav("iflib");
  },
  showView: function(selector, view) {
    if(this.currentView && this.currentView.close) this.currentView.close();
    $(selector).html(view.render());
    this.currentView = view;
    return view;
  },
  showNav: function(sel) {
    var lis = $(".navbar ul.nav li");
    _.each(lis, function(el) {
      if(el) {
        if($(el).find("a").attr("href") == "#"+sel) {
          $(el).addClass("active");
        }
        else {
          $(el).removeClass("active");
        }
      }
    });
  },
  before: function(callback) {
    if(callback) callback.call(this);
  }
});

SC.Models.system.fetch({
  success: function() {
    SC.App = new SC.Router;
    $("a.brand").text(SC.Models.system.attributes.name);
    SC.Models.system.on("change:name", function(model, name) {
      $("a.brand").text(name);
    });
    Backbone.history.start();
  }
});
});
