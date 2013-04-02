SC.Views.DomainList = Backbone.View.extend
  initialize: ->
    this.model.bind 'reset', this.render, this
    this.model.bind 'add', this.appendNewDomain, this
    this.template = _.template $('#domain-list-template').html()

  render: ->
    this.$el.html this.template {}
    this.$ul = this.$el.find("ul").first()
    for domain in this.model.models
      this.appendNewDomain domain
    this.el

  appendNewDomain: (domain) ->
    this.$ul.append(new SC.Views.DomainListItem({model:domain}).render())
